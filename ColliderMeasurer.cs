using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ColliderMeasurer : MonoBehaviour
{

    public static ColliderMeasurer Instance = null;
    private MeshFilter meshFilter = null;

    private void Awake()
    {
        Instance = this;

        meshFilter = GetComponent<MeshFilter>();
    }
    // Start is called before the first frame update
    private void Start()
    {
        transform.rotation = Quaternion.identity;
    }

    public Vector3 Measure(Mesh mesh)
    {
        meshFilter.sharedMesh = mesh;

        Vector3 size = meshFilter.sharedMesh.bounds.size;

        meshFilter.sharedMesh = null;

        return size;
    }

    private void OnValidate()
    {
        if (transform.rotation != Quaternion.identity)
        {
            transform.rotation = Quaternion.identity;
        }
    }


}
