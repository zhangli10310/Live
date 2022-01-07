package com.zl.livepush

import android.Manifest
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import com.zl.livepush.databinding.ActivityPushBinding

class PushActivity : AppCompatActivity() {

    private val TAG = "PushActivity"

    private lateinit var pushBinding: ActivityPushBinding

    private val requestCameraPermission = registerForActivityResult(ActivityResultContracts.RequestPermission()) {
        if (it) {
            startPreview()
        } else {
            Toast.makeText(this, "permission deni", Toast.LENGTH_SHORT).show()
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        pushBinding = ActivityPushBinding.inflate(layoutInflater)
        setContentView(pushBinding.root)


        pushBinding.startPush.setOnClickListener {
            requestCameraPermission.launch(Manifest.permission.CAMERA)
        }
    }

    private fun startPreview() {
        Log.i(TAG, "startPreview: ")
    }
}