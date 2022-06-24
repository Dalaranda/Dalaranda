#include <sampleutil.h>
#include <pad.h>
#include <time.h>

#include <libsysmodule.h>
#include <stdio.h>
#include <kernel.h>

#include "rain.h"
#include "assets.h"

extern "C" size_t sceLibcHeapSize = 256 * 1024 * 1024; // 256 MiB
unsigned int sceLibcHeapExtendedAlloc = 1;

#define APP_DIR "/app0/data/"

namespace vecmath = sce::Vectormath::Simd::Aos;
namespace ssg = sce::SampleUtil::Graphics;


static bool bAudioRunning = true;
static bool bAudioAllow = false;

static void* audioThread(void* pData)
{
    (void)pData;

    while (bAudioRunning) {
        if (bAudioAllow) {
            renderAudio();
            synchronize();
        }
        else {

        }
    }
    return NULL;
}

class Camera
{
public:
    float m_pitch, m_yaw, m_distance;
    vecmath::Vector3 m_target; /* Gaze target of camera */

public:
    Camera()
    {
        m_yaw = 0.0f;
        m_pitch = (float)(0.0 / 180.0 * M_PI);
        m_distance = 500.0f;
        m_target = vecmath::Vector3(0.0f, 80.0f, 0.0f);
    }

    /* Return the camera position */
    vecmath::Vector3 getPosition()
    {
        vecmath::Vector3 z = vecmath::Vector3(0.0f, 0.0f, -1.0f);
        vecmath::Vector3 orientation =
            (vecmath::Matrix4::rotationY(-m_yaw)
                * vecmath::Matrix4::rotationX(-m_pitch)
                * vecmath::Vector4(z, 1.0)).getXYZ();
        return m_target - m_distance * orientation;
    }

    /* Return the view matrix of this camera */
    vecmath::Matrix4 getViewMatrix()
    {
        return vecmath::Matrix4::lookAt(
            vecmath::Point3(getPosition()),
            vecmath::Point3(m_target),
            vecmath::Vector3(0.0f, 1.0f, 0.0f));
    }
};

class Character
{
    ssg::Collada::ColladaData* m_collada;
    ssg::Collada::InstanceVisualScene* m_instanceVisualScene;
    ssg::Collada::AnimationPlayer* m_animPlayer;
public:
    vecmath::Vector3 m_pos;
    float            m_rotY;
    float            m_animationTime;

    int initialize(ssg::Collada::ColladaLoader* loader, const char* path)
    {
        loader->load(&m_collada, path);
        ssg::Collada::createInstanceVisualScene(&m_instanceVisualScene, m_collada->getVisualScene());
        ssg::Collada::createAnimationPlayer(&m_animPlayer, m_instanceVisualScene, m_collada->getAnimation());

        m_animPlayer->setTime(0.0f);

        m_pos = vecmath::Vector3(0, 0, 0);
        m_rotY = 0.0f;
        m_animationTime = 0.0f;

        return SCE_OK;
    }

    int finalize()
    {
        sce::SampleUtil::destroy(m_animPlayer);
        sce::SampleUtil::destroy(m_instanceVisualScene);
        sce::SampleUtil::destroy(m_collada);
        return SCE_OK;
    }

    void update()
    {
        m_animPlayer->setTime(m_animationTime);
        m_animationTime += 0.01f;
        if (m_animationTime >= m_animPlayer->getAnimation()->getEndTime()) {
            m_animationTime = 0.0f;
        }

        /*m_rotY += 0.01f;
        if (m_rotY >= 2.0f * M_PI) {
            m_rotY -= 2.0f * M_PI;
        }*/ // disable character rotation
    }

    void draw(ssg::GraphicsContext* context)
    {
        vecmath::Matrix4 m = vecmath::Matrix4::translation(m_pos)
            * vecmath::Matrix4::rotationZYX(vecmath::Vector3(0.0, m_rotY, 0.0f));
        m_instanceVisualScene->draw(context, m);
    }
};

class Application : public sce::SampleUtil::SampleSkeleton
{
    ssg::Collada::ColladaLoader* m_loader;
    vecmath::Matrix4             m_projectionMatrix;
    Character                    m_boy;
    Character m_boy2;
    Character m_boy3;
    Camera                       m_camera;
    int m_handle = SCE_SAMPLE_UTIL_USER_ID_INVALID; 
    ScePadData m_data[8];
    int i = 0;

public:
    virtual int initialize()
    {    
        initializeUtil();
        SceUserServiceUserId userId; 
        sceUserServiceGetInitialUser(&userId);
        scePadInit();
        m_handle = scePadOpen(userId, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);


        sce::SampleUtil::Graphics::RenderTarget* render_target = getGraphicsContext()->getNextRenderTarget();

        ssg::Collada::createColladaLoader(&m_loader, getGraphicsContext());

        const float fov = (float)(M_PI / 6.0f);
        const float aspect = (float)render_target->getWidth() / (float)render_target->getHeight();
        const float _near = 0.1f;
        const float _far = 10000.0f;
        m_projectionMatrix = vecmath::Matrix4::perspective(fov, aspect, _near, _far);

        vecmath::Vector3 lightPos(0, 1200, 1000);
        vecmath::Vector3 lightColour(1.0, 1.0, 1.0);

        m_loader->getDefaultParams()->setLight(lightPos, lightColour);

        m_boy.initialize(m_loader, APP_DIR "graphics/model/boy/astroBoy_walk.dae");

        m_boy2.initialize(m_loader, APP_DIR "graphics/model/boy/astroBoy_walk.dae");// second instance of m_boy
        m_boy3.initialize(m_loader, APP_DIR "graphics/model/boy/astroBoy_walk.dae");// third instance of m_boy

        m_boy2.m_animationTime = m_boy2.m_animationTime + 0.5f;// second instance of m_boy animation offset
        m_boy3.m_animationTime = m_boy2.m_animationTime + 1.0f;// third instance of m_boy animation offset

        //initilization of audio and creation of thread
        sceSysmoduleLoadModule(SCE_SYSMODULE_AUDIO_3D);

        initializeSample(SCE_USER_SERVICE_USER_ID_SYSTEM);

        scePthreadCreate(&audioThreadHandle, NULL, audioThread, NULL, "audioThread");    
       

        return SCE_OK;
    }

    virtual int update()
    {       
        // increment of i varible upon call to update function at i = 200 light colour gets randomised and i is set back to 0
        i++;
        if (i >= 200) {
            vecmath::Vector3 lightColour((rand()) / static_cast <float>  (RAND_MAX), (rand()) / static_cast <float> (RAND_MAX), (rand()) / static_cast <float> (RAND_MAX));
            vecmath::Vector3 lightPos(0, 1200, 1000);
            m_loader->getDefaultParams()->setLight(lightPos, lightColour);
            i = 0;
        }
       
        updateUtil();
        m_boy.update();
        m_boy2.m_pos = vecmath::Vector3(100,0,-50);   // position of m_boy2 offset    
        m_boy2.update();  
        m_boy3.m_pos = vecmath::Vector3(-100, 0, -50);// position of m_boy3 offset
        m_boy3.update();

        scePadRead(m_handle, m_data, sizeof(m_data) / sizeof(ScePadData));
        // program close button set to x on ps4 controller 
        if (m_data[0].buttons & SCE_PAD_BUTTON_CROSS) {
            return -1;
        } 

        // toggle audio play on and off, o on ps4 controller
        if (m_data[0].buttons & SCE_PAD_BUTTON_CIRCLE) { 
            if (bAudioAllow != true) {
                bAudioAllow = true;
            }
            else if (bAudioAllow) {
                bAudioAllow = false;
            }                               
        }
      
        // camera yaw controller via the left analog stick
        if (m_data[0].leftStick.x > 128) {
            m_camera.m_yaw = m_camera.m_yaw + 0.1f;
        }

        if (m_data[0].leftStick.x < 128) {
            m_camera.m_yaw = m_camera.m_yaw - 0.1f;
        }
            return SCE_OK;
        
        
        

        
    }

    virtual void render()
    {
        m_loader->getDefaultParams()->setProjectionMatrix(m_projectionMatrix);
        getGraphicsContext()->beginScene(getGraphicsContext()->getNextRenderTarget(ssg::GraphicsContext::kFrameBufferSideLeft), getGraphicsContext()->getDepthStencilSurface());

        getGraphicsContext()->clearRenderTarget(0x00000000); 

        getGraphicsContext()->setDepthWriteEnable(true);
        getGraphicsContext()->setDepthFunc(ssg::kDepthFuncLessEqual);

        m_loader->getDefaultParams()->setViewMatrix(m_camera.getViewMatrix());
        getGraphicsContext()->setCullMode(ssg::kCullNone);
        m_boy.draw(getGraphicsContext());

        m_boy2.draw(getGraphicsContext());
        m_boy3.draw(getGraphicsContext());

        getGraphicsContext()->setDepthFunc(ssg::kDepthFuncAlways);

        getGraphicsContext()->endScene();
        getGraphicsContext()->flip(1);

    }

    virtual int finalize()
    {
        bAudioRunning = false;
        scePthreadJoin(audioThreadHandle, NULL);

        finalizeSample();

        sceSysmoduleUnloadModule(SCE_SYSMODULE_AUDIO_3D);


        m_boy.finalize();
        m_boy2.finalize();
        m_boy3.finalize();
        scePadClose(m_handle);
        sce::SampleUtil::destroy(m_loader);
        finalizeUtil();

        return SCE_OK;
    }
    private:
        ScePthread audioThreadHandle;
};


int main(int argc, char* argv[])
{
    int ret = 0;
    Application app;
    ret = app.initialize();

    while (1) {
        ret = app.update();
        if (ret != SCE_OK) {
            break;
        }
        
        app.render();
    }

    ret = app.finalize();
    return 0;
}