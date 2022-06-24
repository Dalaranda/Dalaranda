#include "vec.hpp"
#include "draw-triangle-pro.hpp"
#include "raylib-cpp.hpp"
#include <vector>
#include <cmath>  // std::atan, std::fmod, std::abs
#include <random> // std::random_device, std::mt19937
#include <algorithm> // std::clamp
#include <variant> // std::variant, std::visit
#include "raylib.h"
using Vector = ai::Vector3;  // use x and z in the 2D case

float randomBinomial()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution < float > dis(0.0f, 1.0f);
    return dis(gen);
};
// These two values each represent an acceleration.
// i.e. they effect changes in velocity (linear and angular).
class SteeringOutput
{
public:
  Vector linear_;
  float angular_;

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
    orientation_ = std::fmod(orientation_, 2*PI); // (-2pi,2pi) - not crucial?

    velocity_ += steering.linear_ * time;
    rotation_ += steering.angular_* time;

    post_process(drag, maxSpeed, time);
  }

  void post_process(const float drag, const float maxSpeed, const float time)
  {
//    if (velocity_.length() > 0)
//      orientation_ = std::atan2(-velocity_.x, velocity_.z);

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

  void draw(int screenwidth, int screenheight)
  {
    const float w = 10, len = 30; // ship width and length
    const ai::Vector2 l{0, -w}, r{0, w}, nose{len, 0};
    ai::Vector2 pos{k_.position_.z, k_.position_.x};
    float ori = -k_.orientation_ * RAD2DEG; // negate: anticlockwise rot

    ai::DrawTrianglePro(pos, l, r, nose, ori, col_);
  }
};

// Dynamic Seek (page 96)
class Seek
{
public:
  Kinematic& character_;
  Kinematic& target_;

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

class Align
{
public:
    Kinematic& character_;
    Kinematic& target_;

    float maxAngularAcceleration;
    float maxRotation;

    float targetRadius;

    float slowRadius;

    float timeToTarget;

    SteeringOutput getSteering() const
    {
        SteeringOutput result;
        character_.rotation_ = target_.orientation_ - character_.orientation_;

        character_.rotation_ = std::abs(character_.rotation_) > PI ? character_.rotation_ - 2 * PI : character_.rotation_;
        auto rotationSize = abs(character_.rotation_);

        if (rotationSize < targetRadius) {
            
        } 

        if (rotationSize > slowRadius) {
            target_.rotation_ = maxRotation;
        }
        else {
            target_.rotation_ = maxRotation * rotationSize / slowRadius;
        }
              
        target_.rotation_ *= character_.rotation_ / rotationSize;

        result.angular_ = target_.rotation_ - character_.rotation_;
        result.angular_ /= timeToTarget;

        auto angularAcceleration = abs(result.angular_);
        if (angularAcceleration > maxAngularAcceleration)
        {
            result.angular_ /= angularAcceleration;
            result.angular_ *= maxAngularAcceleration;
        }
        
        result.linear_ = 0;
        return result;

       
    }
};

class Face : Align {
public:
    Kinematic& target_;
    Kinematic& chracter_;

    SteeringOutput getSteering() const {
        auto direction = target_.position_ - character_.position_;
        
        if (direction.length() == 0) {
            return getSteering();
        }

        Align::target_ = target_;
        Align::target_.orientation_ = atan2(-direction.x, direction.z);
        return Align::getSteering();
    }

};

class Wander //: Face
{
public:
    Kinematic& target_;
    Kinematic& character_;
    Align& wanderOrientation;

    float wanderOffset;
    float wanderRadius;

    float wanderRate;

    float maxAcceleration;

    SteeringOutput getSteering() const {

        wanderOrientation.target_.orientation_+= randomBinomial() * wanderRate ;

        auto targetOrientation = wanderOrientation.target_.orientation_ + target_.orientation_;

        auto target = target_.position_ + wanderOffset * ai::asVector(target_.orientation_);

        target += wanderRadius * ai::asVector(targetOrientation);

        SteeringOutput result;

        result.linear_ = maxAcceleration * ai::asVector(target_.orientation_);

        return result;
    }
};

class BlendedSteering
{
   /* class BehaviorAndWeight {
        using SteeringBehaviour = std::variant<Seek, Align>;
        float weight;

        SteeringBehaviour behaviors[2];

        float maxAcceleration;
        float maxRotation;

        SteeringOutput getSteering() const {
            SteeringBehaviour result;

            for (auto i : behaviors) {
                result. += weight * SteeringBehaviour(i);

            }

            result = result.linear_, maxAcceleration;
            result.angular_ = result.angular_, maxRotation;
            return result;
        }
    }; */

};

int main(int argc, char *argv[])
{  
  int w{1024}, h{768};
  raylib::Window window(w, h, "Game AI: Assignment 1");

  float windowW = window.GetWidth();
  float windowH = window.GetHeight();

  SetTargetFPS(60);

  Ship hunter{w/2.0f + 50, h/2.0f, 0, RED};
  Ship prey{w/2.0f + 250, h/2.0f  + 300 , 270*DEG2RAD, BLUE};

  Rectangle boxA = { w / 2.0f + 50, h / 2.0f, 20, 20 };
  Rectangle boxB = { w / 2.0f + 250, h / 2.0f + 300, 20, 20 };

  Rectangle boxCollision = { 0 };
  bool collision = false;

  InitAudioDevice();

  Sound sound1 = LoadSound("weird.wav");

  float target_radius{5};
  float slow_radius{60};
  const float max_accel{200};
  const float max_ang_accel{10};
  const float max_speed{220};
  const float drag_factor{0.5};

  Seek seek{ hunter.k_, prey.k_, 1000 };

  Align align{ hunter.k_, prey.k_, max_ang_accel, 1, 0.01, 0.1 };
 
  Wander wander{ prey.k_,hunter.k_, align,  300, 20, 0.04, 700};

  while (!window.ShouldClose()) // Detect window close button or ESC key
  {
      int preyW = prey.k_.position_.x;
      int preyH = prey.k_.position_.z;
      int huntW = hunter.k_.position_.x;
      int huntH = hunter.k_.position_.z;
      if (preyW > windowH) {
          prey.k_.position_.x = 0;
      }

      if (preyW < 0) {
          prey.k_.position_.x = windowH ;
      }

      if (preyH > windowW) {
          prey.k_.position_.z = 0;
      }

      if (preyH < 0) {
          prey.k_.position_.z = windowW;
      }

      if (huntW > windowH) {
          hunter.k_.position_.x = 0;
      }
      if ( huntW < 0) {
          hunter.k_.position_.x = windowH;
      }
      if (huntH > windowW) {
          hunter.k_.position_.z = 0;
      }
      if ( huntH < 0) {
          hunter.k_.position_.z = windowW ;
      }

      collision = CheckCollisionRecs(boxA, boxB);

      if (collision) { 
          boxCollision = GetCollisionRec(boxA, boxB); 
         PlaySound(sound1);
         prey.k_.position_ = randomBinomial()  * windowW ;
      }
     
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      const auto mpos = GetMousePosition();
    }

    prey.draw(w,h);
    hunter.draw(w,h);

    //DrawRectangleRec(boxA, GOLD);
   // DrawRectangleRec(boxB, BLUE);

    if (collision) {
        DrawRectangleRec(boxCollision, LIME);
    }
    

    EndDrawing();

    auto steer = seek.getSteering();
    auto steer2 = align.getSteering();
    auto steer3 = wander.getSteering();
    //auto steer2 = wander.getSteering();

    hunter.k_.update(steer, max_speed, drag_factor, GetFrameTime());
    hunter.k_.update(steer2, max_speed, drag_factor, GetFrameTime());
    boxA.x = hunter.k_.position_.x;
    boxA.y = hunter.k_.position_.z;
    boxB.x = prey.k_.position_.x;
    boxB.y = prey.k_.position_.z;
    prey.k_.update(steer3, max_speed, drag_factor, GetFrameTime());
    
  }
  UnloadSound(sound1);

  CloseAudioDevice();

  return 0;
}
