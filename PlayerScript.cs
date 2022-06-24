using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using UnityEngine.SceneManagement;

public class PlayerScript : MonoBehaviour
{
    public GameObject ScoreBoard;

    public GameObject player1Score;
    public GameObject player2Score;
    public GameObject player3Score;
    public GameObject player4Score;
    public GameObject player5Score;
    public GameObject player6Score;
    public GameObject player7Score;
    public GameObject player8Score;
    public GameObject player9Score;
    public GameObject player10Score;

    public int player2ScoreNum = 3;
    public int player3ScoreNum = 1;
    public int player4ScoreNum = 1;
    public int player5ScoreNum = 1;
    public int player6ScoreNum = 3;
    public int player7ScoreNum = 5;
    public int player8ScoreNum = 2;
    public int player9ScoreNum = 2;
    public int player10ScoreNum = 4;

    public int StickSelected = 0;


    public GameObject RightHandedStick;
    public GameObject LeftHandedStick;

    public GameObject LocationToSpawner;

    public GameObject UIController;

    public GameObject Player;
    public GameObject PlayerWarpLoc1;
    public GameObject PlayerWarpLoc2;

    public bool LeftHandCanGrabTablet = false;
    public bool RightHandCanGrabTablet = false;
    public bool onEnd = false;

    public int currentDay = 0;
    public int CurrentScore = 0;
    public int RoundScore = 0;
    // Start is called before the first frame update
    void Start()
    {
        if (SceneManager.GetActiveScene().name == "EndScene")
        {
            onEnd = true;
        }
    }

    public void LocationToSpawnerFunc()
    {
        if (StickSelected == 0)
        {
            RightHandedStick.transform.position = LocationToSpawner.transform.position;
        }
        if(StickSelected == 1)
        {
            LeftHandedStick.transform.position = LocationToSpawner.transform.position;
        }
    }

    IEnumerator ExecuteWarp1AfterTime()
    {
        yield return new WaitForSeconds(0.01f);

        if (Player.transform.position != PlayerWarpLoc1.transform.position)
        {
            AfterFadeToBlack();
        }
    }

    IEnumerator ExecuteWarp2AfterTime()
    {
        yield return new WaitForSeconds(0.01f);

        if (Player.transform.position != PlayerWarpLoc2.transform.position)
        {
            MovePlayerToLoc2();
        }
    }


    public void AfterFadeToBlack()
    {

        Player.transform.position = PlayerWarpLoc1.transform.position;
        StartCoroutine(ExecuteWarp1AfterTime());
        UIController.GetComponent<UIController>().returnFade();
        
    }

    public void MovePlayerToLoc2()
    {
        
        Player.transform.position = PlayerWarpLoc2.transform.position;
        StartCoroutine(ExecuteWarp2AfterTime());
    }

    // Update is called once per frame
    void Update()
    {
        TextMeshPro scoreText1 = ScoreBoard.GetComponent<TextMeshPro>();
        if (onEnd != true)
        {


            TextMeshProUGUI playerScoreText1 = player1Score.GetComponent<TextMeshProUGUI>();
            TextMeshProUGUI playerScoreText2 = player2Score.GetComponent<TextMeshProUGUI>();
            TextMeshProUGUI playerScoreText3 = player3Score.GetComponent<TextMeshProUGUI>();
            TextMeshProUGUI playerScoreText4 = player4Score.GetComponent<TextMeshProUGUI>();
            TextMeshProUGUI playerScoreText5 = player5Score.GetComponent<TextMeshProUGUI>();
            TextMeshProUGUI playerScoreText6 = player6Score.GetComponent<TextMeshProUGUI>();
            TextMeshProUGUI playerScoreText7 = player7Score.GetComponent<TextMeshProUGUI>();
            TextMeshProUGUI playerScoreText8 = player8Score.GetComponent<TextMeshProUGUI>();
            TextMeshProUGUI playerScoreText9 = player9Score.GetComponent<TextMeshProUGUI>();
            TextMeshProUGUI playerScoreText10 = player10Score.GetComponent<TextMeshProUGUI>();

            //scoreText1.text = "Player Score: " + RoundScore.ToString();
            playerScoreText1.text = "Score: " + CurrentScore.ToString();
            playerScoreText2.text = "Score: " + player2ScoreNum.ToString();
            playerScoreText3.text = "Score: " + player3ScoreNum.ToString();
            playerScoreText4.text = "Score: " + player4ScoreNum.ToString();
            playerScoreText5.text = "Score: " + player5ScoreNum.ToString();
            playerScoreText6.text = "Score: " + player6ScoreNum.ToString();
            playerScoreText7.text = "Score: " + player7ScoreNum.ToString();
            playerScoreText8.text = "Score: " + player8ScoreNum.ToString();
            playerScoreText9.text = "Score: " + player9ScoreNum.ToString();
            playerScoreText10.text = "Score: " + player10ScoreNum.ToString();
        }
        scoreText1.text = "Player Score: " + RoundScore.ToString();
    }
}
