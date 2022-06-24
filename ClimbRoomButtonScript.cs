using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ClimbRoomButtonScript : MonoBehaviour
{
    public GameObject buttonLock;
    public GameObject buttonLock2;
    public GameObject buttonLock3;
    public GameObject buttonLock4;
    public GameObject buttonLock5;

    public Gun gunScript;
    public Gun pistolScript;
    public Gun shotgunScript;

    public float timerTime = 0.0f;
    public float timer2Time = 1.0f;
    public float timer3Time = 0.0f;

    public bool isTimerRunning = false;
    public bool isTimer2Running = false;
    public bool isTimer3Running = false;

    public void StartTimer()
    {
        isTimerRunning = true;
        timerTime = 30.0f;
        gunScript.doorIsLocked = false;
        pistolScript.doorIsLocked = false;
        shotgunScript.doorIsLocked = false;
    }

    public void lockDoor()
    {
        isTimerRunning = false;
        gunScript.doorIsLocked = true;
        pistolScript.doorIsLocked = true;
        shotgunScript.doorIsLocked = true;
    }

    public void StartTimer2()
    {
        isTimer2Running = true;
        timer2Time = 20.0f;
        gunScript.doorIsLocked = true;
        pistolScript.doorIsLocked = true;
        shotgunScript.doorIsLocked = true;
    }
    public void lockDoor2()
    {
        isTimer2Running = false;
        gunScript.doorIsLocked = false;
        pistolScript.doorIsLocked = false;
        shotgunScript.doorIsLocked = false;
    }
    public void endTimer2()
    {
        isTimer2Running = false;
        timer2Time = 0f;
        gunScript.doorIsLocked = false;
        pistolScript.doorIsLocked = false;
        shotgunScript.doorIsLocked = false;
    }

    public void StartTimer3()
    {
        isTimer3Running = true;
        timer3Time = 40.0f;
        gunScript.doorIsLocked = true;
        pistolScript.doorIsLocked = true;
        shotgunScript.doorIsLocked = true;
    }
    public void lockDoor3()
    {
        isTimer3Running = false;
        gunScript.doorIsLocked = false;
        pistolScript.doorIsLocked = false;
        shotgunScript.doorIsLocked = false;
    }


    void Update()
    {
        if (timerTime > 0)
        {
            timerTime -= Time.deltaTime;
        }
        if (timerTime < 0)
        {
            lockDoor();
        }

        if (timer2Time > 0)
        {
            timer2Time -= Time.deltaTime;

        }
        if (timer2Time < 0)
        {
            lockDoor2();
        }

        if (timer3Time > 0)
        {
            timer3Time -= Time.deltaTime;

        }
        if (timer3Time < 0)
        {
            lockDoor3();
        }
    }
}
