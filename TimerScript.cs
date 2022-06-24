using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;

public class TimerScript : MonoBehaviour
{
    public float text;
    public float text2;

    public Text textClock;

    public GameObject timerButton;

    private void Start()
    {
        textClock = GetComponent<Text>();
    }

    void Update()
    {
        ClimbRoomButtonScript timerthing = timerButton.GetComponent<ClimbRoomButtonScript>();


        text = timerthing.timerTime;
        text2 = timerthing.timer2Time;

        if (timerthing.isTimerRunning == true)
        {
            textClock.enabled = true;
            textClock.text = "Time: " + text.ToString("f1");
        }
        else if(timerthing.isTimerRunning == false)
        {
            textClock.enabled = false;
        }      

    }
}
