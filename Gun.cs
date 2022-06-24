using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;
using TMPro;

public class Gun : MonoBehaviour
{
    public DoorScript doorScript;
    public DoorScript1 doorScript1;
    public DoorScript2 doorScript2;
    public DoorScript3 doorScript3;
    public DoorScript4 doorScript4;
    public DoorScript5 doorScript5;
    public DoorScript6 doorScript6;
    public DoorScript7 doorScript7;
    public DoorScript8 doorScript8;

    public ClimbRoomButtonScript climbroombuttonscript;
    public EndGameScript endGame;

    public AudioSource audioSource;
    public AudioClip audioClip;
    public float volume = 0.5f;

    public Animation shotgun;
    public Animation AK;
    public Animation pistol;
    public int weaponselected;
    public Animator animator;

    public float damage = 10f;
    public float range = 100f;
    public float impactForce = 100f;
    public float fireRate = 15f;

    public int maxAmmo = 10;
    private int currentAmmo;
    public float reloadTime = 1f;
    private bool isReloading = false;

    public Camera fpsCam;
    public ParticleSystem muzzleFlash;
    public GameObject impactEffect;
    public Text ammocount;
    public float text;

    public GameObject room1text;
    public GameObject room2text;
    public GameObject room3text;
    public GameObject room4text;
    public GameObject room5text;
    public GameObject room6text;
    public GameObject room7text;

    public float maxRnd = 10;
    public float minRnd = 3;
    public float shotRnd = 3;
    public float recoilAmt = 5;
    public float recoilRecoveryRate = 4;
    bool ReloadAudioPlaying = false;

    public int createsHit = 0;
    public bool hasRedKey = false;
    public bool hasBlueKey = false;
    public bool hasGreenKey = false;

    public bool doorIsLocked = false;

    private float nextTimeToFire = 0f;

    // Start is called before the first frame update
    void Start()
    {
        doorScript = GameObject.Find("Door").GetComponent<DoorScript>();
        doorScript1 = GameObject.Find("Door1").GetComponent<DoorScript1>();
        doorScript2 = GameObject.Find("Door2").GetComponent<DoorScript2>();
        doorScript3 = GameObject.Find("Door3").GetComponent<DoorScript3>();
        doorScript4 = GameObject.Find("Door4").GetComponent<DoorScript4>();
        doorScript5 = GameObject.Find("Door5").GetComponent<DoorScript5>();
        doorScript6 = GameObject.Find("Door6").GetComponent<DoorScript6>();
        doorScript7 = GameObject.Find("Door7").GetComponent<DoorScript7>();
        doorScript8 = GameObject.Find("Door8").GetComponent<DoorScript8>();
        climbroombuttonscript = GameObject.Find("ClimbRoomButton").GetComponent<ClimbRoomButtonScript>();
        WeaponSwitching weaponHolder = GameObject.Find("WeaponHolder").GetComponent<WeaponSwitching>();
        endGame = GameObject.Find("EndGameButton").GetComponent<EndGameScript>();
        
        
        
        
        weaponselected = weaponHolder.selectedWeapon;
        currentAmmo = maxAmmo;
    }

    void OnEnable()
    {
        isReloading = false;
        animator.SetBool("Reloading", false);
    }

    // Update is called once per frame
    void Update()
    {
        if (weaponselected == 0)
        {
            Animation shotgun = GameObject.Find("Shotgun").GetComponent<Animation>();
            Gun ammoThing = GetComponent<Gun>();
            text = ammoThing.currentAmmo;

            ammocount.GetComponent<Text>();
            ammocount.text = "Ammo: " +  text.ToString()+"/6";
            if (isReloading == true && ReloadAudioPlaying == false)
            {
                ReloadAudioPlaying = true;
                audioClip = Resources.Load("ShotgunReload") as AudioClip;
                audioSource = GetComponent<AudioSource>();
                audioSource.PlayOneShot(audioClip, volume);
            }
        }
        else if(weaponselected == 1)
        {
            Animation AK = GameObject.Find("AK-47").GetComponent<Animation>();
            Gun ammoThing = GetComponent<Gun>();
            text = ammoThing.currentAmmo;

            ammocount.GetComponent<Text>();
            ammocount.text = "Ammo: " + text.ToString() + "/31";
            if (isReloading == true && ReloadAudioPlaying == false)
            {
                ReloadAudioPlaying = true;
                audioClip = Resources.Load("AKReload") as AudioClip;
                audioSource = GetComponent<AudioSource>();
                audioSource.PlayOneShot(audioClip, volume);
            }
        }
        else if(weaponselected == 2)
        {
            Animation pistol = GameObject.Find("Handgun").GetComponent<Animation>();
            Gun ammoThing = GetComponent<Gun>();
            text = ammoThing.currentAmmo;

            ammocount.GetComponent<Text>();
            ammocount.text = "Ammo: " + text.ToString() + "/12";            
            if (isReloading == true && ReloadAudioPlaying == false)
            {
                ReloadAudioPlaying = true;
                audioClip = Resources.Load("PistolReload") as AudioClip;
                audioSource = GetComponent<AudioSource>();
                audioSource.PlayOneShot(audioClip, volume);
            }
        }
        if (hasGreenKey == true)
        {
            Debug.Log("you have the greeen key");
        }
        if (isReloading)
        {
            return;
        }
        if(currentAmmo <= 0)
        {
            StartCoroutine(Reload());
            return;
        }
        if(createsHit == 8)
        {
            hasRedKey = true;
            climbroombuttonscript.endTimer2();

            TextMeshPro roomtext1 = room1text.GetComponent<TextMeshPro>();
            TextMeshPro roomtext6 = room6text.GetComponent<TextMeshPro>();

            roomtext1.text = "Congratulations you have passed the test and recived RedLevel access procede to the hall on your left and enter the vent system.";
            roomtext6.text = "Your sense of direction is terrible has anyone ever told you, please return to the last room and follow the directions better";
        }
        if(hasGreenKey && hasRedKey)
        {
            TextMeshPro roomtext1 = room1text.GetComponent<TextMeshPro>();
            TextMeshPro roomtext3 = room3text.GetComponent<TextMeshPro>();
            TextMeshPro roomtext4 = room4text.GetComponent<TextMeshPro>();
            TextMeshPro roomtext5 = room5text.GetComponent<TextMeshPro>();
            TextMeshPro roomtext6 = room6text.GetComponent<TextMeshPro>();

            roomtext1.text = "Procede to the last test take the vent to the room below";
            roomtext3.text = "Now you are ready take the door to your right and begin the final test";
            roomtext4.text = "No sense putting it off go back and start the final test";
            roomtext5.text = "Congratulations i am very proud of you but you still have 1 test remaining please procede down the hallway";
            roomtext6.text = "There was going to be a 4th test in this room but the budget got spent on developing rocket munitions." + "" + "please procede to your right";
        }
        if(hasBlueKey)
        {
            TextMeshPro roomtext7 = room7text.GetComponent<TextMeshPro>();

            roomtext7.text = "You actually survived amazing procede through the door on the right and shoot the end test button.";
        }
        if (Input.GetButton("Fire1") && Time.time >= nextTimeToFire)
        {          
            nextTimeToFire = Time.time + 1f / fireRate;
            Shoot();
        }
        shotRnd = Mathf.MoveTowards(shotRnd, minRnd, recoilRecoveryRate * Time.deltaTime);

        // print(shotRnd);
       Debug.Log(createsHit);
        
    } 

    IEnumerator Reload()
    {
        isReloading = true;
        //Debug.Log("Realoding...");      
        animator.SetBool("Reloading", true);

        yield return new WaitForSeconds(reloadTime - .25f);
        animator.SetBool("Reloading", false);
        yield return new WaitForSeconds(.25f);

        currentAmmo = maxAmmo;
        isReloading = false;
        ReloadAudioPlaying = false;
        
    }

    void Shoot()
    {
        muzzleFlash.Play();

        currentAmmo--;
        if (weaponselected == 0)
        {
            shotgun["Reload"].wrapMode = WrapMode.Once;
            shotgun.Play("Reload");
            audioClip = Resources.Load("ShotgunShot") as AudioClip;
            audioSource = GetComponent<AudioSource>();
            audioSource.PlayOneShot(audioClip, volume);
        }
        else if(weaponselected == 1)
        {
            AK["Shoot"].wrapMode = WrapMode.Once;
            AK.Play("Shoot");
            audioClip = Resources.Load("Gunshot2") as AudioClip;
            audioSource = GetComponent<AudioSource>();
            audioSource.PlayOneShot(audioClip, volume);
        }
        else if(weaponselected ==2)
        {
            pistol["Shoot"].wrapMode = WrapMode.Once;
            pistol.Play("Shoot");
            audioClip = Resources.Load("PistolShot") as AudioClip;
            audioSource = GetComponent<AudioSource>();
            audioSource.PlayOneShot(audioClip, volume);
        }

        
        Vector3 noAngle = fpsCam.transform.forward;
        

        Quaternion spreadAngle = Quaternion.Euler(Random.Range(-shotRnd, shotRnd), Random.Range(-shotRnd, shotRnd), 0);

       

        Vector3 newVector = spreadAngle * noAngle;
        RaycastHit hit;
        if(Physics.Raycast(fpsCam.transform.position, newVector, out hit, range))
        {
            shotRnd = Mathf.Clamp(shotRnd + recoilAmt, minRnd, maxRnd);
            var hitName = hit.transform.name;

            /*GameObject obj = GameObject.CreatePrimitive(PrimitiveType.Cube);
            obj.transform.position = hit.point;
            obj.transform.up = hit.normal;
            obj.transform.Rotate(0, Random.Range(0, 360), 0);
            Destroy(obj, 5);*/
            GameObject impactGO = Instantiate(impactEffect, hit.point, Quaternion.LookRotation(hit.normal));
            impactGO.transform.position = hit.point;
            impactGO.transform.up = hit.normal;
            //impactGO.transform.Rotate(0, Random.Range(0, 360), 0);            
            Destroy(impactGO, 5);

           

            Target target = hit.transform.GetComponent<Target>();
            //Debug.Log(hitName);
            if(target != null)
            {
                target.TakeDamage(damage);
            }

            if(hitName == "CardboardBox1(Clone)")
            {
                createsHit++;
            }

            if (hitName == "CardboardBox2(Clone)")
            {
                createsHit = 0;
            }

            if (hitName == "FirstRoomButton")
            {
                createsHit = 0;
                climbroombuttonscript.StartTimer2();
            }

            if(hitName == "ClimbRoomButton")
            {
                climbroombuttonscript.StartTimer();
            }

            if(hitName == "RocketRoomButton")
            {
                hasBlueKey = true;
                climbroombuttonscript.StartTimer3();
            }

            if(hitName == "EndGameButton")
            {
                Cursor.visible = true;
                Cursor.lockState = CursorLockMode.None;
                endGame.EndGameScreen();
            }

            if(hitName == "DoorButton" || hitName == "DoorButton0-2")
            {               
                doorScript.openDoor();
                
            }

            if (hitName == "DoorButton1" && hasBlueKey == true && climbroombuttonscript.isTimer3Running == false || hitName == "DoorButton1-2")
            {
                doorScript1.openDoor1();

            }

            if (hitName == "DoorButton2" && hasRedKey == true && hasGreenKey == true || hitName == "DoorButton2-2" && doorIsLocked == false)
            {
                doorScript2.openDoor2();
            }
            else
            {
                Debug.Log("add you need other keys message");
            }

            if (hitName == "DoorButton3" && doorIsLocked == false || hitName == "DoorButton3-2")
            {
                doorScript3.openDoor3();
            }

            if (hitName == "DoorButton4" || hitName == "DoorButton4-2" && doorIsLocked == false)
            {
                doorScript4.openDoor4();
            }

            if (hitName == "DoorButton5" || hitName == "DoorButton5-2")
            {
                doorScript5.openDoor5();
            }

            if (hitName == "DoorButton6" || hitName == "DoorButton6-2")
            {
                doorScript6.openDoor6();
            }

            if (hitName == "DoorButton7" || hitName == "DoorButton7-2")
            {
                doorScript7.openDoor7();
            }

            if (hitName == "DoorButton8" || hitName == "DoorButton8-2" && doorIsLocked == false )
            {
                doorScript8.openDoor8();
            }

            if(hitName == "BlueKey")
            {
                hasBlueKey = true;
            }
            if (hitName == "RedKey")
            {
                hasBlueKey = true;
            }
            if (hitName == "GreenKey")
            {
                hasBlueKey = true;
            }

            if (hit.rigidbody != null)
            {
                hit.rigidbody.AddForce(-hit.normal * impactForce);
            }
            
            //Destroy(impactGO, 2f);

        }
    }
}
