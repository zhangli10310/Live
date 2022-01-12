package com.zl.livepush

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.camera.core.Preview
import androidx.camera.core.impl.ImageAnalysisConfig
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.content.ContextCompat
import com.zl.livepush.databinding.ActivityPushBinding

class PushActivity : AppCompatActivity() {

    private val TAG = "PushActivity"

    private lateinit var pushBinding: ActivityPushBinding

    val cameraPermission = registerForActivityResult(ActivityResultContracts.RequestPermission()) {
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
            if (ContextCompat.checkSelfPermission(
                    this,
                    Manifest.permission.CAMERA
                ) == PackageManager.PERMISSION_GRANTED
            ) {
                startPreview()
            } else {
                cameraPermission.launch(Manifest.permission.CAMERA)
            }
        }
    }

    private fun startPreview() {
        Log.i(TAG, "startPreview: ")
        val providerFuture = ProcessCameraProvider.getInstance(this)
        providerFuture.addListener({
            val provider = providerFuture.get()
            val preview = Preview.Builder()
                .build()
            val cameraSelector = CameraSelector.Builder()
                .requireLensFacing(CameraSelector.LENS_FACING_BACK)
                .build()
            val analysis = ImageAnalysis.Builder().build()
            analysis.setAnalyzer(ContextCompat.getMainExecutor(this), {
                push(it)
                it.close()
            })
            try {
                provider.unbindAll()
                provider.bindToLifecycle(this, cameraSelector, preview, analysis)
                preview.setSurfaceProvider(pushBinding.previewView.surfaceProvider)
//                preview.setSurfaceProvider {
//                    it.provideSurface()
//                }
            } catch (e: Exception) {
                e.printStackTrace()
            }

        }, ContextCompat.getMainExecutor(this))
    }

    private fun push(imageProxy: ImageProxy) {

    }
}