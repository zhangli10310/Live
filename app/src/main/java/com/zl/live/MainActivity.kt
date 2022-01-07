package com.zl.live

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.zl.glnative.GLActivity
import com.zl.live.databinding.ActivityMainBinding
import com.zl.livepush.PushActivity
import com.zl.mediacodec.Mp4Activity

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.toGL.setOnClickListener {
            startActivity(Intent(this, GLActivity::class.java))
        }
        binding.toMp4.setOnClickListener {
            startActivity(Intent(this, Mp4Activity::class.java))
        }
        binding.toPush.setOnClickListener {
            startActivity(Intent(this, PushActivity::class.java))
        }


    }

}