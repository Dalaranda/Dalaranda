#include "vec.hpp"
#include "draw-triangle-pro.hpp"
#include "raylib-cpp.hpp"
#include "raylib.h"
#include <math.h>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <cmath>     // std::atan, std::fmod, std::abs
#include <random>    // std::random_device, std::mt19937
#include <variant>   // std::variant, std::visit
#include <iostream>  // std::cout, std::cerr
#include <chrono>
#include <time.h>
#include <ctime>

#define MAX_SCORE_CIRCLES 15
#define MAX_HAZZARD_CIRCLES 15

using Vector = ai::Vector3;  // use x and z in the 2D case

class SteeringOutput
{
public:
  Vector linear_; // These two values each represent an acceleration.
  float angular_; // i.e. they effect changes in velocity (linear and angular).

  SteeringOutput &operator+=(const SteeringOutput &rhs)
  {
    linear_ += rhs.linear_;
    angular_ += rhs.angular_;
    return *this;
  }
  friend SteeringOutput operator*(const float lhs, const SteeringOutput &y) {
    return {lhs*y.linear_, lhs*y.angular_};
  }
};

class Kinematic
{
public:
  Vector position_;
  float orientation_;
  Vector velocity_;
  float rotation_;

  // integration of the linear and angular accelerations
  void update(const SteeringOutput& steering,
              const float maxSpeed,
              float drag,
              const float time) // delta time
  {
    //Newton-Euler 1 simplification:
    position_ += velocity_ * time;
    orientation_ += rotation_ * time;
    orientation_ = std::fmod(orientation_, 2*PI);

    velocity_ += steering.linear_ * time;
    rotation_ += steering.angular_* time;

    post_process(drag, maxSpeed, time);
  }

  void post_process(const float drag, const float maxSpeed, const float time)
  {
    velocity_ *= (1 - drag * time);
    rotation_ *= (1 - drag * time);

    if (velocity_.length() > maxSpeed)
    {
      velocity_.normalise();
      velocity_ *= maxSpeed;
    }
  }
};

class Ship
{
public:
  Ship(const float z, const float x, const float ori, const raylib::Color col)
    : k_{{x,0,z},ori,{0,0,0},0}, col_{col} { }

  Kinematic k_;
  raylib::Color col_;

  void update(const SteeringOutput& steering,
              const float maxSpeed,
              float drag,
              const float time) { k_.update(steering,maxSpeed,drag,time); }

  void draw(int screenwidth, int screenheight)
  {
    const float w = 10, len = 30; // ship width and length
    const ai::Vector2 l{0, -w}, r{0, w}, nose{len, 0};
    ai::Vector2 pos{k_.position_.z, k_.position_.x};
    float ori = -k_.orientation_ * RAD2DEG; // negate: anticlockwise rot

    // wrap
    /*
    pos.x = std::fmod(pos.x, static_cast<float>(screenwidth));
    pos.y = std::fmod(pos.y, static_cast<float>(screenheight));
    pos.x = pos.x < 0 ? pos.x + screenwidth : pos.x;
    pos.y = pos.y < 0 ? pos.y + screenheight : pos.y;
    */
    //teleport
    if (k_.position_.z > screenwidth) k_.position_.z = 0;

    else if (k_.position_.z < 0) k_.position_.z = screenwidth;

    if (k_.position_.x > screenheight) k_.position_.x = 0;

    else if (k_.position_.x < 0) k_.position_.x = screenheight;

    ai::DrawTrianglePro(pos, l, r, nose, ori, col_);
  }
};

// Dynamic Seek (page 96)
class Seek
{
public:
  const Kinematic& character_;
  const Kinematic& target_;

  float maxAcceleration_;

  /* // A constructor isn't needed, but adding it will also not hurt
  Seek(Kinematic &c, Kinematic &t, float maxAcceleration)
    : character_{c}, target_{t}, maxAcceleration{maxAcceleration_}
  {
  }*/

  SteeringOutput getSteering() const
  {
    SteeringOutput result;

    result.linear_ = target_.position_ - character_.position_;

    result.linear_.normalise();
    result.linear_ *= maxAcceleration_;

    result.angular_ = 0;
    return result;
  }
};

// Based on Wander
class Onward
{
public:

  const Kinematic& character;

  float maxAcceleration_;
  float maxAngularAcceleration_;
  float headForce_;

  void setHeadForce(const float headForce) { headForce_ = headForce; }

  SteeringOutput getSteering() const
  {
    SteeringOutput result;

    result.linear_ = maxAcceleration_ * ai::asVector(character.orientation_);
    result.angular_ = headForce_ * maxAngularAcceleration_;
    return result;
  };
};

class Align
{
public:
  const Kinematic& character_;
  const Kinematic& target_;

  float maxAngularAcceleration_;
  float maxRotation_;

  float targetRadius_;
  float slowRadius_;

  float timeToTarget_ = 0.1f;

  SteeringOutput getSteering() const
  {
    SteeringOutput result{}; // a zero value

    float rotation = target_.orientation_ - character_.orientation_;

    // mapToRange(rotation); (-pi,pi)
    rotation = std::abs(rotation) > PI ? rotation-2*PI : rotation;
    float rotationSize = std::abs(rotation);
    float targetRotation;

    if (rotationSize < targetRadius_)
      return result;

    if (rotationSize > slowRadius_)
      targetRotation = maxRotation_;
    else
      targetRotation = maxRotation_ * rotationSize / slowRadius_;

    targetRotation *= rotation / rotationSize;

    result.angular_ = targetRotation - character_.rotation_;
    result.angular_ /= timeToTarget_;

    float angularAcceleration = std::abs(result.angular_);
    if (angularAcceleration > maxAngularAcceleration_)
    {
      result.angular_ /= angularAcceleration;
      result.angular_ *= maxAngularAcceleration_;
    }

    result.linear_ = 0;
    return result;
  }
};

class BlendedSteering
{
public:
  class BehaviourAndWeight
  {
  public:
    using SteeringBehaviour = std::variant<Seek,Align>;
    SteeringBehaviour behaviour_;
    float weight_;
  };

  std::vector<BehaviourAndWeight> behaviours_;

  float maxAcceleration_;
  float maxAngularAcceleration_;

  SteeringOutput getSteering() const
  {
    SteeringOutput result{};

    for (auto &b : behaviours_)
      result +=
        b.weight_ *
        std::visit([](auto &b) { return b.getSteering(); }, b.behaviour_);

    if (result.linear_.length() > maxAcceleration_)
    {
      result.linear_.normalise();
      result.linear_ *= maxAcceleration_;
    }

    float angularAcceleration = std::abs(result.angular_);
    if (angularAcceleration > maxAngularAcceleration_)
    {
      result.angular_ /= angularAcceleration;
      result.angular_ *= maxAngularAcceleration_;
    }

    return result;
  }
};

class timer {
public:
    timer();
    void           start();
    void           stop();
    void           reset();
    bool           isRunning();
    unsigned long  getTime();
    bool           isOver(unsigned long seconds);
private:
    bool           resetted;
    bool           running;
    unsigned long  beg;
    unsigned long  end;
};

timer::timer() {
    resetted = true;
    running = false;
    beg = 0;
    end = 0;
}

void timer::start() {
    if (!running) {
        if (resetted)
            beg = (unsigned long)clock();
        else
            beg -= end - (unsigned long)clock();
        running = true;
        resetted = false;
    }
}


void timer::stop() {
    if (running) {
        end = (unsigned long)clock();
        running = false;
    }
}


void timer::reset() {
    bool wereRunning = running;
    if (wereRunning)
        stop();
    resetted = true;
    beg = 0;
    end = 0;
    if (wereRunning)
        start();
}

bool timer::isRunning() {
    return running;
}


unsigned long timer::getTime() {
    if (running)
        return ((unsigned long)clock() - beg) / CLOCKS_PER_SEC;
    else
        return end - beg;
}


bool timer::isOver(unsigned long seconds) {
    return seconds >= getTime();
}

typedef struct sCircle {
    Vector2 position;
    float radius;
    bool active;
    Color color;
} sCircle;

typedef struct hCircle {
    Vector2 position;
    float radius;
    bool active;
    Color color;
} hCircle;

static sCircle scoreCircle[MAX_SCORE_CIRCLES] = { 0 };
static hCircle hazzardCircle[MAX_HAZZARD_CIRCLES] = { 0 };

int main(int argc, char* argv[])
{
    int w{ 1024 }, h{ 768 };
    raylib::Window window(w, h, "Ai Assignment 2");
    raylib::Color grey(LIGHTGRAY);

    raylib::AudioDevice audiodevice;
    raylib::Sound fx("../resources/weird.wav");
    raylib::Sound fx2("../resources/coin.wav");
    raylib::Sound fx3("../resources/spring.wav");

    SetTargetFPS(60);

    Ship enemy{ w / 2.0f + 50, h / 2.0f, 0, RED };
    Ship player{ w / 2.0f + 250, h / 2.0f + 300, 270 * DEG2RAD, BLUE };

    float target_radius{ 5 };
    float slow_radius{ 60 };
    const float max_accel{ 200 };
    const float max_ang_accel{ 10 };
    const float max_speed{ 220 };
    const float drag_factor{ 0.5 };
    const float collision_distance{ 10 };

    int livesL = 3;
    int currentScore = 0;
    int highScore = 0;

    int q = 0;

    int n = 0;

    unsigned long seconds = 3;
    timer t;
    timer t2;

    Seek seek{ enemy.k_, player.k_, 1000 };
    Align align{ enemy.k_, player.k_, max_ang_accel, 1, 0.01, 0.1 };
    Onward onward{ player.k_, max_accel, max_ang_accel, 0 };
    BlendedSteering blend{ {{seek,0.5},{align,0.5}}, max_accel, max_ang_accel };

    cv::VideoCapture vid(0);
    if (!vid.isOpened()) {
        std::cerr << "error: Camera 0 could not be opened for capture.\n";
        return -1;
    }

    cv::Mat  frame, mini_frame;
    cv::UMat gray, prevgray, uflow; // Change to cv::Mat if you see OpenCL errors

    float posz, posx;

    for (int i = 0; i < MAX_SCORE_CIRCLES; i++)
    {
        posz = GetRandomValue(0, w);
        if (posz > w / 2 - 150 && posz < w / 2 + 150) posz = GetRandomValue(0, w);
        posx = GetRandomValue(0, h);
        if (posx > h / 2 - 150 && posx < h / 2 + 150) posx = GetRandomValue(0, h);

        scoreCircle[i].position = Vector2{ posz, posx };
        scoreCircle[i].radius = 15;
        scoreCircle[i].active = true;
        scoreCircle[i].color = DARKGREEN;

        hazzardCircle[i].position = Vector2{ posz,posx };
        hazzardCircle[i].radius = 5;
        hazzardCircle[i].active = false;
        hazzardCircle[i].color = BLACK;
    }

    while (!window.ShouldClose()) // Detect window close button or ESC key
    {
        vid >> frame;
        cv::resize(frame, mini_frame, cv::Size(), 1, 1); // half res: speed up
        cvtColor(mini_frame, gray, cv::COLOR_BGR2GRAY);

        if (!prevgray.empty())
        {
            calcOpticalFlowFarneback(prevgray, gray, uflow, 0.1, 5, 15, 3, 5, 1.1, 0);
            cv::Scalar v = cv::mean(uflow);
            onward.setHeadForce(v[0]);
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        auto sep = player.k_.position_ - enemy.k_.position_;
        if (sep.length() < collision_distance) {
            fx.Play();
            livesL--;
            player.k_.position_ = rand() % 1024 + 1, rand() % 768 + 1; //player respawn after collision not 100% likely to be away from collision area
        }

        t.start();


        if (t.getTime() == seconds) {
            n++;

            t.reset();
        }                       

        for (int i = 0; i < n; i++)
        {
            if (scoreCircle[i].active)DrawCircleV(scoreCircle[i].position, scoreCircle[i].radius, DARKGREEN);

            if (CheckCollisionCircles(Vector2{ player.k_.position_.z, player.k_.position_.x }, 30.0f, scoreCircle[i].position, scoreCircle[i].radius) && scoreCircle[i].active)
            {
                fx2.Play();
                currentScore = currentScore + 10;
                scoreCircle[i].active = false;
                q = i;
                t2.start();
            }

            if (t2.getTime() == 1) {
                
                    //hazzardsToSpawn[i].active = true;
                    hazzardCircle[q].active = true;

                t2.reset();
                t2.stop();
            }

                if (hazzardCircle[i].active) {
                    DrawCircleV(hazzardCircle[i].position, hazzardCircle[i].radius, BLACK);
                }

                if (CheckCollisionCircles(Vector2{ player.k_.position_.z, player.k_.position_.x }, 30.0f, hazzardCircle[i].position, hazzardCircle[i].radius) && hazzardCircle[i].active) {
                    fx.Play();
                    livesL--;
                    player.k_.position_ = rand() % 1024 + 1, rand() % 768 + 1; //player respawn after collision not 100% likely to be away from collision area
                }

                if (CheckCollisionCircles(Vector2{ enemy.k_.position_.z, enemy.k_.position_.x }, 30.0f, hazzardCircle[i].position, hazzardCircle[i].radius) && hazzardCircle[i].active) {
                    fx3.Play();
                    currentScore = currentScore + 20;
                    enemy.k_.position_ = rand() % 1024 + 1, rand() % 768 + 1; //enemy respawn after collision not 100% likely to be away from collision area
                }
            }          
        

            if (currentScore > highScore) {
                highScore = currentScore;
            }

            if (livesL < 0) {
                currentScore = 0;
                livesL = 3;
                t.reset();
                n = 0;
                scoreCircle->active = false;
                hazzardCircle->active = false;

                for (int i = 0; i < MAX_SCORE_CIRCLES; i++)
                {
                    posz = GetRandomValue(0, w);
                    if (posz > w / 2 - 150 && posz < w / 2 + 150) posz = GetRandomValue(0, w);
                    posx = GetRandomValue(0, h);
                    if (posx > h / 2 - 150 && posx < h / 2 + 150) posx = GetRandomValue(0, h);

                    scoreCircle[i].position = Vector2{ posz, posx };
                    scoreCircle[i].radius = 15;
                    scoreCircle[i].active = true;
                    scoreCircle[i].color = DARKGREEN;

                    hazzardCircle[i].position = Vector2{ posz,posx };
                    hazzardCircle[i].radius = 5;
                    hazzardCircle[i].active = false;
                    hazzardCircle[i].color = BLACK;
                }
            }


            player.draw(w, h);
            enemy.draw(w, h);

            grey.DrawText("Score: " + std::to_string(currentScore), 10, h - 35, 30); //currentScore
            grey.DrawText("HighScore: " + std::to_string(highScore), w - 300, 5, 30);//high score
            grey.DrawText("Lives: " + std::to_string(livesL), 10, 5, 30);//num of lives remaining
            grey.DrawText("CircleSpawnIn: " + std::to_string(t.getTime()), w - 300, h - 35, 30);

            EndDrawing();

            auto se = blend.getSteering();
            auto sp = onward.getSteering();
            player.update(sp, max_speed - 60, drag_factor, GetFrameTime());
            enemy.update(se, max_speed, drag_factor, GetFrameTime());

            std::swap(prevgray, gray);
        }


        return 0;
    }

