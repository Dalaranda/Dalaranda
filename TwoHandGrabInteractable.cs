using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.Interaction.Toolkit;

public class TwoHandGrabInteractable : XRGrabInteractable
{
    public List<XRSimpleInteractable> secondHandGrabPoints = new List<XRSimpleInteractable>();
    private XRBaseInteractor secondInteractor;
    private Quaternion attachInitialRotation;
    public enum TwoHandRotationType { None, First, Second };
    public TwoHandRotationType twoHandRotationType;

    public GameObject ResetLocationObject;

    public BoxCollider OutofBounds;
    // Start is called before the first frame update
    void Start()
    {
        foreach (var item in secondHandGrabPoints)
        {
            item.onSelectEnter.AddListener(OnSecondHandGrab);
            item.onSelectExit.AddListener(OnSecondHandRelease);
        }
    }

    // Update is called once per frame
    void Update()
    {

    }

    public override void ProcessInteractable(XRInteractionUpdateOrder.UpdatePhase updatePhase)
    {
        if (secondInteractor && selectingInteractor)
        {
            //Compute rotation
            selectingInteractor.attachTransform.rotation = GetTwoHandRotation();
        }
        base.ProcessInteractable(updatePhase);
    }

    private Quaternion GetTwoHandRotation()
    {
        Quaternion targetRotation = Quaternion.LookRotation(secondInteractor.attachTransform.position - selectingInteractor.attachTransform.position);
        if (twoHandRotationType == TwoHandRotationType.None)
        {
            targetRotation = Quaternion.LookRotation(secondInteractor.attachTransform.position - selectingInteractor.attachTransform.position);
        }
        else if (twoHandRotationType == TwoHandRotationType.First)
        {
            targetRotation = Quaternion.LookRotation(secondInteractor.attachTransform.position - selectingInteractor.attachTransform.position, selectingInteractor.attachTransform.up);
        }
        else if (twoHandRotationType == TwoHandRotationType.Second)
        {
            targetRotation = Quaternion.LookRotation(secondInteractor.attachTransform.position - selectingInteractor.attachTransform.position, secondInteractor.attachTransform.up);
        }

        return targetRotation;
    }

    public void OnSecondHandGrab(XRBaseInteractor interactor)
    {
        Debug.Log("SECOND HAND GRAB");
        secondInteractor = interactor;
    }

    public void OnSecondHandRelease(XRBaseInteractor interactor)
    {
        Debug.Log("SECOND HAND RELEASE");
        secondInteractor = null;
    }

    protected override void OnSelectEntering(XRBaseInteractor interactor)
    {
        Debug.Log("First Grab Enter");
        base.OnSelectEntering(interactor);
        attachInitialRotation = interactor.attachTransform.localRotation;
    }

    protected override void OnSelectExiting(XRBaseInteractor interactor)
    {
        Debug.Log("First Grab Exit");
        base.OnSelectExiting(interactor);
        secondInteractor = null;
        interactor.attachTransform.localRotation = attachInitialRotation;
    }

    public override bool IsSelectableBy(XRBaseInteractor interactor)
    {
        bool isalreadygrabbed = selectingInteractor && !interactor.Equals(selectingInteractor);
        return base.IsSelectableBy(interactor) && !isalreadygrabbed;
    }

    void OnTriggerEnter(Collider col)
    {
        if (col == OutofBounds)
        {
            ResetLocation();
            Debug.Log("Out of Bounds");
        }
    }

    public void ResetLocation()
    {
        gameObject.transform.position = ResetLocationObject.transform.position;
        gameObject.GetComponent<Rigidbody>().velocity = Vector3.zero;
        gameObject.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
    }
}
