/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package org.cocos2dx.lib;

import java.io.*;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.UnsupportedEncodingException;
import java.lang.Exception;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.Locale;
import java.lang.Runnable;
import java.nio.channels.FileChannel;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.*;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.WindowManager;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.util.Log;

public class Cocos2dxHelper {
	// ===========================================================
	// Constants
	// ===========================================================
	private static final String PREFS_NAME = "Cocos2dxPrefsFile";
	private static final int RUNNABLES_PER_FRAME = 5;

	// ===========================================================
	// Fields
	// ===========================================================

	private static Cocos2dxMusic sCocos2dMusic;
	private static Cocos2dxSound sCocos2dSound;
	private static AssetManager sAssetManager;
	private static Cocos2dxAccelerometer sCocos2dxAccelerometer;
	private static boolean sAccelerometerEnabled;
	private static String sPackageName;
	private static String sFileDirectory;
    private static String sCachePath;
	private static Activity sActivity = null;
	private static Cocos2dxHelperListener sCocos2dxHelperListener;
	private static ConcurrentLinkedQueue<Runnable> jobs = new ConcurrentLinkedQueue<Runnable>();

    /**
     * Optional meta-that can be in the manifest for this component, specifying
     * the name of the native shared library to load.  If not specified,
     * "main" is used.
     */
    private static final String META_DATA_LIB_NAME = "android.app.lib_name";
    private static final String DEFAULT_LIB_NAME = "main";

	// ===========================================================
	// Constructors
	// ===========================================================

	public static void dispatchPendingRunnables() {
		for (int i = RUNNABLES_PER_FRAME; i > 0; i--) {
			Runnable job = jobs.poll();
			if (job == null) {
				return;
			}
			job.run();
		}
	}

	public static void runOnGLThread(final Runnable r) {
		jobs.add(r);
	}

	private static boolean sInited = false;
	public static void init(final Activity activity) {
	    if (!sInited) {
    		final ApplicationInfo applicationInfo = activity.getApplicationInfo();
    		
            Cocos2dxHelper.sCocos2dxHelperListener = (Cocos2dxHelperListener)activity;
                
            try {
            // Get the lib_name from AndroidManifest.xml metadata
                ActivityInfo ai =
                    activity.getPackageManager().getActivityInfo(activity.getIntent().getComponent(), PackageManager.GET_META_DATA);
                if (null != ai.metaData) {
                    String lib_name = ai.metaData.getString(META_DATA_LIB_NAME);
                    if (null != lib_name) {
                        System.loadLibrary(lib_name);
                    } else {
                        System.loadLibrary(DEFAULT_LIB_NAME);
                    }
                }
            } catch (PackageManager.NameNotFoundException e) {
                throw new RuntimeException("Error getting activity info", e);
            }
    
    		Cocos2dxHelper.sPackageName = applicationInfo.packageName;
    		Cocos2dxHelper.sFileDirectory = activity.getFilesDir().getAbsolutePath();
            Cocos2dxHelper.nativeSetApkPath(applicationInfo.sourceDir);
    
    		Cocos2dxHelper.sCocos2dxAccelerometer = new Cocos2dxAccelerometer(activity);
    		Cocos2dxHelper.sCocos2dMusic = new Cocos2dxMusic(activity);
    		Cocos2dxHelper.sCocos2dSound = new Cocos2dxSound(activity);
    		Cocos2dxHelper.sAssetManager = activity.getAssets();
    		Cocos2dxHelper.nativeSetContext((Context)activity, Cocos2dxHelper.sAssetManager);
    
            Cocos2dxBitmap.setContext(activity);
            Cocos2dxETCLoader.setContext(activity);
            sActivity = activity;

            sCachePath = isExternalStorageUsable() ? getExternalCachePath() : getInternalCachePath();

            sInited = true;

	    }
	}
	
    public static Activity getActivity() {
        return sActivity;
    }

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	// ===========================================================
	// Methods
	// ===========================================================

	private static native void nativeSetApkPath(final String pApkPath);

	private static native void nativeSetEditTextDialogResult(final byte[] pBytes);

    public static boolean moveFile(String srcPath, String dstPath)
    {
        boolean success = false;
        File src = new File(srcPath);
        File dst = new File(dstPath);

        FileChannel inChannel = null, outChannel = null;

        try {
            inChannel = new FileInputStream(src).getChannel();
            outChannel = new FileOutputStream(dst).getChannel();

            inChannel.transferTo(0, inChannel.size(), outChannel);
            src.delete();
            success = true;
        }
        catch (java.io.IOException e) {
        }
        finally {
            if (inChannel != null) {
                try {
                    inChannel.close();
                }
                catch (java.io.IOException e) {
                }
            }

            if (outChannel != null) {
                try {
                    outChannel.close();
                }
                catch (java.io.IOException e) {
                }
            }
        }
        return success;
    }

    public static String getCachePath() {
        return sCachePath;
    }

    public static boolean isExternalStorageUsable()
    {
        boolean externalStorageAvailable = false;
        boolean externalStorageWriteable = false;
        String state = Environment.getExternalStorageState();

        if (Environment.MEDIA_MOUNTED.equals(state)) {
            // We can read and write the media
            externalStorageAvailable = externalStorageWriteable = true;
        } else if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
            // We can only read the media
            externalStorageAvailable = true;
            externalStorageWriteable = false;
        } else {
            // Something else is wrong. It may be one of many other states, but all we need
            //  to know is we can neither read nor write
            externalStorageAvailable = externalStorageWriteable = false;
        }

        return externalStorageAvailable && externalStorageWriteable;
    }

    public static boolean isInstalledOnExternalStorage()
    {
        PackageManager pm = sActivity.getPackageManager();
        try {
            PackageInfo pi = pm.getPackageInfo(sActivity.getPackageName(), 0);
            ApplicationInfo ai = pi.applicationInfo;
            return (ai.flags & ApplicationInfo.FLAG_EXTERNAL_STORAGE) == ApplicationInfo.FLAG_EXTERNAL_STORAGE;
        } catch (NameNotFoundException e) {
            Log.d("cocos2dx-helper", "Failed to : " + e.getMessage());
        }
        return false;
    }

    public static String getExternalDocPath()
    {
        File f = sActivity.getExternalFilesDir(null);
        return f == null ? "" : f.getAbsolutePath();
    }

    public static String getExternalCachePath()
    {
        File f = sActivity.getExternalCacheDir();
        return f == null ? "" : f.getAbsolutePath();
    }

    public static String getInternalDocPath()
    {
        File f = sActivity.getFilesDir();
        return f.getAbsolutePath();
    }

    public static String getInternalCachePath()
    {
        File f = sActivity.getCacheDir();
        return f.getAbsolutePath();
    }

	private static native void nativeSetContext(final Context pContext, final AssetManager pAssetManager);

	public static String getCocos2dxPackageName() {
		return Cocos2dxHelper.sPackageName;
	}

	public static String getCocos2dxWritablePath() {
		return Cocos2dxHelper.sFileDirectory;
	}

	public static String getCurrentLanguage() {
		return Locale.getDefault().getLanguage();
	}
	
	public static String getDeviceModel(){
		return Build.MODEL;
    }

	public static AssetManager getAssetManager() {
		return Cocos2dxHelper.sAssetManager;
	}

	public static void enableAccelerometer() {
		Cocos2dxHelper.sAccelerometerEnabled = true;
		Cocos2dxHelper.sCocos2dxAccelerometer.enable();
	}


	public static void setAccelerometerInterval(float interval) {
		Cocos2dxHelper.sCocos2dxAccelerometer.setInterval(interval);
	}

	public static void disableAccelerometer() {
		Cocos2dxHelper.sAccelerometerEnabled = false;
		Cocos2dxHelper.sCocos2dxAccelerometer.disable();
	}

	public static void preloadBackgroundMusic(final String pPath) {
		Cocos2dxHelper.sCocos2dMusic.preloadBackgroundMusic(pPath);
	}

	public static void playBackgroundMusic(final String pPath, final boolean isLoop) {
		Cocos2dxHelper.sCocos2dMusic.playBackgroundMusic(pPath, isLoop);
	}

	public static void resumeBackgroundMusic() {
		Cocos2dxHelper.sCocos2dMusic.resumeBackgroundMusic();
	}

	public static void pauseBackgroundMusic() {
		Cocos2dxHelper.sCocos2dMusic.pauseBackgroundMusic();
	}

	public static void stopBackgroundMusic() {
		Cocos2dxHelper.sCocos2dMusic.stopBackgroundMusic();
	}

	public static void rewindBackgroundMusic() {
		Cocos2dxHelper.sCocos2dMusic.rewindBackgroundMusic();
	}

	public static boolean isBackgroundMusicPlaying() {
		return Cocos2dxHelper.sCocos2dMusic.isBackgroundMusicPlaying();
	}

	public static float getBackgroundMusicVolume() {
		return Cocos2dxHelper.sCocos2dMusic.getBackgroundVolume();
	}

	public static void setBackgroundMusicVolume(final float volume) {
		Cocos2dxHelper.sCocos2dMusic.setBackgroundVolume(volume);
	}

	public static void preloadEffect(final String path) {
		Cocos2dxHelper.sCocos2dSound.preloadEffect(path);
	}

	public static int playEffect(final String path, final boolean isLoop, final float pitch, final float pan, final float gain) {
		return Cocos2dxHelper.sCocos2dSound.playEffect(path, isLoop, pitch, pan, gain);
	}

	public static void resumeEffect(final int soundId) {
		Cocos2dxHelper.sCocos2dSound.resumeEffect(soundId);
	}

	public static void pauseEffect(final int soundId) {
		Cocos2dxHelper.sCocos2dSound.pauseEffect(soundId);
	}

	public static void stopEffect(final int soundId) {
		Cocos2dxHelper.sCocos2dSound.stopEffect(soundId);
	}

	public static float getEffectsVolume() {
		return Cocos2dxHelper.sCocos2dSound.getEffectsVolume();
	}

	public static void setEffectsVolume(final float volume) {
		Cocos2dxHelper.sCocos2dSound.setEffectsVolume(volume);
	}

	public static void unloadEffect(final String path) {
		Cocos2dxHelper.sCocos2dSound.unloadEffect(path);
	}

	public static void pauseAllEffects() {
		Cocos2dxHelper.sCocos2dSound.pauseAllEffects();
	}

	public static void resumeAllEffects() {
		Cocos2dxHelper.sCocos2dSound.resumeAllEffects();
	}

	public static void stopAllEffects() {
		Cocos2dxHelper.sCocos2dSound.stopAllEffects();
	}

	public static void end() {
		Cocos2dxHelper.sCocos2dMusic.end();
		Cocos2dxHelper.sCocos2dSound.end();
	}

	public static void onResume() {
		if (Cocos2dxHelper.sAccelerometerEnabled) {
			Cocos2dxHelper.sCocos2dxAccelerometer.enable();
		}
	}

	public static void onPause() {
		if (Cocos2dxHelper.sAccelerometerEnabled) {
			Cocos2dxHelper.sCocos2dxAccelerometer.disable();
		}
	}

	public static void terminateProcess() {
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	private static void showDialog(final String pTitle, final String pMessage) {
		Cocos2dxHelper.sCocos2dxHelperListener.showDialog(pTitle, pMessage);
	}

	private static void showEditTextDialog(final String pTitle, final String pMessage, final int pInputMode, final int pInputFlag, final int pReturnType, final int pMaxLength) {
		Cocos2dxHelper.sCocos2dxHelperListener.showEditTextDialog(pTitle, pMessage, pInputMode, pInputFlag, pReturnType, pMaxLength);
	}

	public static void setEditTextDialogResult(final String pResult) {
		try {
			final byte[] bytesUTF8 = pResult.getBytes("UTF8");

			Cocos2dxHelper.sCocos2dxHelperListener.runOnGLThread(new Runnable() {
				@Override
				public void run() {
					Cocos2dxHelper.nativeSetEditTextDialogResult(bytesUTF8);
				}
			});
		} catch (UnsupportedEncodingException pUnsupportedEncodingException) {
			/* Nothing. */
		}
	}

    public static int getDPI()
    {
		if (sActivity != null)
		{
			DisplayMetrics metrics = new DisplayMetrics();
			WindowManager wm = sActivity.getWindowManager();
			if (wm != null)
			{
				Display d = wm.getDefaultDisplay();
				if (d != null)
				{
					d.getMetrics(metrics);
					return (int)(metrics.density*160.0f);
				}
			}
		}
		return -1;
    }
    
    // ===========================================================
 	// Functions for CCUserDefault
 	// ===========================================================
    
    public static boolean getBoolForKey(String key, boolean defaultValue) {
    	SharedPreferences settings = sActivity.getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
    	return settings.getBoolean(key, defaultValue);
    }
    
    public static int getIntegerForKey(String key, int defaultValue) {
    	SharedPreferences settings = sActivity.getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
    	return settings.getInt(key, defaultValue);
    }
    
    public static float getFloatForKey(String key, float defaultValue) {
    	SharedPreferences settings = sActivity.getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
    	return settings.getFloat(key, defaultValue);
    }
    
    public static double getDoubleForKey(String key, double defaultValue) {
    	// SharedPreferences doesn't support saving double value
    	SharedPreferences settings = sActivity.getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
    	return settings.getFloat(key, (float)defaultValue);
    }
    
    public static String getStringForKey(String key, String defaultValue) {
    	SharedPreferences settings = sActivity.getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
    	return settings.getString(key, defaultValue);
    }
    
    public static void setBoolForKey(String key, boolean value) {
    	SharedPreferences settings = sActivity.getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
    	SharedPreferences.Editor editor = settings.edit();
    	editor.putBoolean(key, value);
    	editor.commit();
    }
    
    public static void setIntegerForKey(String key, int value) {
    	SharedPreferences settings = sActivity.getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
    	SharedPreferences.Editor editor = settings.edit();
    	editor.putInt(key, value);
    	editor.commit();
    }
    
    public static void setFloatForKey(String key, float value) {
    	SharedPreferences settings = sActivity.getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
    	SharedPreferences.Editor editor = settings.edit();
    	editor.putFloat(key, value);
    	editor.commit();
    }
    
    public static void setDoubleForKey(String key, double value) {
    	// SharedPreferences doesn't support recording double value
    	SharedPreferences settings = sActivity.getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
    	SharedPreferences.Editor editor = settings.edit();
    	editor.putFloat(key, (float)value);
    	editor.commit();
    }
    
    public static void setStringForKey(String key, String value) {
    	SharedPreferences settings = sActivity.getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
    	SharedPreferences.Editor editor = settings.edit();
    	editor.putString(key, value);
    	editor.commit();
    }
	
	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================

	public static interface Cocos2dxHelperListener {
		public void showDialog(final String pTitle, final String pMessage);
		public void showEditTextDialog(final String pTitle, final String pMessage, final int pInputMode, final int pInputFlag, final int pReturnType, final int pMaxLength);

		public void runOnGLThread(final Runnable pRunnable);
	}
}
