package com.zl.glnative

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.opengl.GLSurfaceView
import android.os.BatteryManager
import android.os.Build
import android.os.Bundle
import android.os.PowerManager
import android.util.Log
import android.view.SurfaceHolder
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.zl.glnative.databinding.ActivityGlBinding
import java.io.BufferedReader
import java.io.DataInputStream
import java.io.IOException
import java.io.InputStreamReader
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.concurrent.thread


/**
 *  Created by zhangli04 at 2021/7/12 3:36 下午
 */
class GLActivity : AppCompatActivity() {

    private val TAG = "GLActivity"
    private lateinit var binding: ActivityGlBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityGlBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val renderer = GLRenderer()
//        binding.surfaceView.setRenderer(object : GLSurfaceView.Renderer {
//            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
//                renderer.init()
//            }
//
//            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
//
//            }
//
//            override fun onDrawFrame(gl: GL10?) {
//
//            }
//
//        })
        binding.surfaceView.setOnClickListener {
            Log.i(TAG, "onCreate: ")
        }
        binding.surfaceView.holder.addCallback(object : SurfaceHolder.Callback {
            override fun surfaceCreated(holder: SurfaceHolder) {
                renderer.init(holder.surface)
            }

            override fun surfaceChanged(
                holder: SurfaceHolder,
                format: Int,
                width: Int,
                height: Int
            ) {
                renderer.reset(width, height)
            }

            override fun surfaceDestroyed(holder: SurfaceHolder) {
                renderer.stop()
            }

        })


//        val batInfoReceiver = object : BroadcastReceiver() {
//            override fun onReceive(context: Context?, intent: Intent?) {
//                val t = intent?.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, 0)
//                Log.i(TAG, "onReceive: temperature=$t")
//            }
//
//        }
//        val intent = registerReceiver(batInfoReceiver, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
//        val t = intent?.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, 0)
//        val level = intent?.getIntExtra(BatteryManager.EXTRA_LEVEL, 0)
//        val status = intent?.getIntExtra(BatteryManager.EXTRA_STATUS, -1)
//        Log.i(TAG, "onCreate: t=$t, level=$level, status=$status")
//        val batteryManager = (getSystemService(BATTERY_SERVICE) as? BatteryManager)
//        Log.i(
//            TAG,
//            "onCreate: battery status=${batteryManager?.getIntProperty(BatteryManager.BATTERY_PROPERTY_STATUS)}"
//        )
//
//        val pm = getSystemService(POWER_SERVICE) as PowerManager
//        Log.i(TAG, "onCreate: save=${pm.isPowerSaveMode}")
//
//        getCpuCurGovernor()
        getBatteryInfoByRegister()
        getBatteryInfoByService()
    }

    val cpuFreqPath = "/sys/devices/system/cpu/cpu0/cpufreq"
    fun getCpuCurGovernor() {
        try {
//            val process = Runtime.getRuntime().exec("cat " + cpuFreqPath + "/scaling_governor");
            val process =
                Runtime.getRuntime().exec("cat /sys/class/devfreq/soc:qcom,gpubw/cur_freq");
            val input = BufferedReader(InputStreamReader(process.getInputStream()))
            input.readLine()?.let {
                Log.i(TAG, "getCpuCurGovernor: $it")

            }
            var s: String? = input.readLine()
            do {
                Log.i(TAG, "getCpuCurGovernor: $s")
            } while (s != null)
        } catch (e: Exception) {
            e.printStackTrace();
        }
    }

    fun getBatteryInfoByService() {
        (getSystemService(BATTERY_SERVICE) as? BatteryManager)?.let {
            val capacity = it.getIntProperty(BatteryManager.BATTERY_PROPERTY_CAPACITY)
            val status = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                it.getIntProperty(BatteryManager.BATTERY_PROPERTY_STATUS)
            } else {
                -1
            }
            Log.i(TAG, "getBatteryInfoByService: $capacity, $status")
        }

    }

    fun getBatteryInfoByRegister() {
        val intent = registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        val t = intent?.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, -1)
        val level = intent?.getIntExtra(BatteryManager.EXTRA_LEVEL, -1)
        val scale = intent?.getIntExtra(BatteryManager.EXTRA_SCALE, -1)
        val status = intent?.getIntExtra(BatteryManager.EXTRA_STATUS, -1)
        Log.i(TAG, "getBatteryInfoByRegister: $t, $level, $scale, $status")

        Log.i(TAG, "getBatteryInfoByRegister: charing=${BatteryManager.BATTERY_STATUS_CHARGING}")
        Log.i(TAG, "getBatteryInfoByRegister: not_charging=${BatteryManager.BATTERY_STATUS_NOT_CHARGING}")
        Log.i(TAG, "getBatteryInfoByRegister: full=${BatteryManager.BATTERY_STATUS_FULL}")
        Log.i(TAG, "getBatteryInfoByRegister: charing=${BatteryManager.BATTERY_STATUS_CHARGING}")
    }
}