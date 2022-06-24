using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Pixelplacement;
using UnityEngine.XR.Interaction.Toolkit;

public class WristSocket : XRSocketInteractor
{
    public GameObject TabletRef1;
    public GameObject TabletRef2;
    public GameObject TabletRef3;
    public GameObject TabletRef4;
    public GameObject TabletRef5;
    public GameObject TabletRef6;
    public GameObject TabletRef7;

    public float targetSize = 0.25f;
    public float sizingDuration = 0.25f;

    private Vector3 originalScale = Vector3.one;
    private Vector3 objectSize = Vector3.zero;

    private bool canSelect = false;
    // Start is called before the first frame update
    protected override void OnHoverEntering(XRBaseInteractable interactable)
    {
        base.OnHoverEntering(interactable);

        if (interactable.isSelected)
        {
            canSelect = true;
        }
    }

    protected override void OnHoverExiting(XRBaseInteractable interactable)
    {
        base.OnHoverExiting(interactable);

        if (!selectTarget)
        {
            canSelect = false;
        }
    }

    protected override void OnSelectEntering(XRBaseInteractable interactable)
    {
        base.OnSelectEntering(interactable);
        StoreObjectSizeScale(interactable);
    }

    protected override void OnSelectEntered(XRBaseInteractable interactable)
    {
        if(interactable.tag == "Tablet")
        {
            TabletRef1.GetComponent<ButtonInteractScript>().CanPressButton = false;
            TabletRef2.GetComponent<ButtonInteractScript>().CanPressButton = false;
            TabletRef3.GetComponent<ButtonInteractScript>().CanPressButton = false;
            TabletRef4.GetComponent<ButtonInteractScript>().CanPressButton = false;
            TabletRef5.GetComponent<ButtonInteractScript>().CanPressButton = false;
            TabletRef6.GetComponent<ButtonInteractScript>().CanPressButton = false;
            TabletRef7.GetComponent<ButtonInteractScript>().CanPressButton = false;
        }
        base.OnSelectEntered(interactable);
        TweenSizeToSocket(interactable);
    }

    protected override void OnSelectExited(XRBaseInteractable interactable)
    {      
        base.OnSelectExited(interactable);
        SetOriginalScale(interactable);
        canSelect = false;
    }

    private void StoreObjectSizeScale(XRBaseInteractable interactable)
    {
        objectSize = FindObjectSize(interactable.gameObject);
        originalScale = interactable.transform.localScale;
    }

    private Vector3 FindObjectSize(GameObject objectToMeasure)
    {
        Vector3 size = Vector3.one;

        if (objectToMeasure.TryGetComponent(out MeshFilter meshFilter))
        {
            size = ColliderMeasurer.Instance.Measure(meshFilter.mesh);
        }

        return size;
    }

    private void TweenSizeToSocket(XRBaseInteractable interactable)
    {
        Vector3 targetScale = FindTargetScale();

        Tween.LocalScale(interactable.transform, targetScale, sizingDuration, 0);
    }

    private Vector3 FindTargetScale()
    {
        float largestSize = FindLargestSize(objectSize);
        float scaleDiffrence = targetSize / largestSize;
        return Vector3.one * scaleDiffrence;
    }

    private void SetOriginalScale(XRBaseInteractable interactable)
    {
        if (interactable)
        {
            if (interactable.tag == "Tablet")
            {
                TabletRef1.GetComponent<ButtonInteractScript>().CanPressButton = true;
                TabletRef2.GetComponent<ButtonInteractScript>().CanPressButton = true;
                TabletRef3.GetComponent<ButtonInteractScript>().CanPressButton = true;
                TabletRef4.GetComponent<ButtonInteractScript>().CanPressButton = true;
                TabletRef5.GetComponent<ButtonInteractScript>().CanPressButton = true;
                TabletRef6.GetComponent<ButtonInteractScript>().CanPressButton = true;
                TabletRef7.GetComponent<ButtonInteractScript>().CanPressButton = true;
            }
            interactable.transform.localScale = originalScale;

            originalScale = Vector3.one;
            objectSize = Vector3.zero;
        }
    }

    private float FindLargestSize(Vector3 value)
    {
        float largestSize = Mathf.Max(value.x, value.y);
        largestSize = Mathf.Max(largestSize, value.z);
        return largestSize;
    }

    public override XRBaseInteractable.MovementType? selectedInteractableMovementTypeOverride
    {
        get { return XRBaseInteractable.MovementType.Instantaneous; }
    }

    public override bool isSelectActive => base.isSelectActive && canSelect;

    private void OnDrawGizmos()
    {
        Gizmos.DrawWireSphere(transform.position, targetSize * 0.5f);
    }
}
