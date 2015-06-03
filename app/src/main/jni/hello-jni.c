/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <jni.h>
#include <sys/inotify.h>
#include <pthread.h>
#include <android/log.h>

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */


jstring
Java_pl_pelotasplus_actionafteruninstall_MainActivity_stringFromJNI(JNIEnv* env, jobject thiz) {
    return (*env)->NewStringUTF(env, "Hello from JNI !  Compiled with ABI foo.");
}

#define EVENT_SIZE  (sizeof (struct inotify_event) )
#define BUF_LEN     (1024 * (EVENT_SIZE + 16) )

#define TAG "Native"
#define DIRECTORY   "/data/data/pl.pelotasplus.actionafteruninstall"

void startIntent(void) {
	pid_t p = fork();
	if (p == 0) {
		__android_log_print(ANDROID_LOG_INFO, TAG, "startIntent %d", getpid());

//			        execlp("am", "am", "start", "-a android.intent.action.VIEW", "-d http://droidcon.de", NULL);
//			        execlp("top", "/system/bin/top", NULL);
//			        execlp("/system/bin/am", "am", "start", "com.android.browser", NULL);
//			        execlp("/system/bin/am", "am", "start", "-a android.intent.action.VIEW", "-d http://droidcon.de", NULL);
//			        system("/system/bin/am start com.android.browser &", NULL);
        system("/system/bin/am start --user 0 -a android.intent.action.VIEW -d http://droidcon.de");
//        system("/data/data/start.sh");

        __android_log_print(ANDROID_LOG_INFO, TAG, "After startintent %d", getpid());
    } else {
    	__android_log_print(ANDROID_LOG_INFO, TAG, "startIntent %d", getpid());
    }
}

void observer(void) {
	int length, i = 0;
	int fd;
	int wd;
	char buffer[BUF_LEN];

	__android_log_print(ANDROID_LOG_INFO, "Native", "My PID %d", getpid());

	fd = inotify_init();
    __android_log_print(ANDROID_LOG_INFO, "Native", "fd=%d", fd);

	if (fd < 0) {
		perror("inotify_init");
	}

	wd = inotify_add_watch(fd, DIRECTORY, IN_DELETE | IN_CREATE);
    __android_log_print(ANDROID_LOG_INFO, TAG, "Watching %s\n", DIRECTORY);
	length = read(fd, buffer, BUF_LEN);
    __android_log_print(ANDROID_LOG_INFO, TAG, "Read %d\n", length);
	if (length < 0) {
		perror("read");
	}

	while (i < length) {
		struct inotify_event *event = (struct inotify_event *) &buffer[ i ];
		printf("Event len %d\n", event->len);
		if (event->len) {
			if (event->mask & IN_CREATE) {
			    __android_log_print(ANDROID_LOG_INFO, TAG, "%s was created\n", event->name);
			} else if (event->mask & IN_DELETE) {
			    __android_log_print(ANDROID_LOG_INFO, TAG, "%s was deleted\n", event->name);

				// only lib and cache?
				if (event->mask & IN_ISDIR) {
				    // am start -a android.intent.action.VIEW -d http://droidcon.de

					startIntent();
				}
			}
		}
		i += EVENT_SIZE + event->len;
	}

	(void) inotify_rm_watch(fd, wd);
	(void) close(fd);
}

static void *observer_thread(void *arg) {
    observer();
}

pthread_t thread;
pthread_attr_t attr;

/*
	/data/app-lib/com.opera.max-1/libuo.so -mtrue -uhttps://bugs.opera.com/wizard/max-uninstall?platform_ver=4.4.2&device_name=unknown%20sdk%20generic&build=0.9.714.1&mcc=310260 -pcom.opera.max -ccom.android.browser/com.android.browser.BrowserActivity -t1433282539
*/

void
Java_pl_pelotasplus_actionafteruninstall_MainActivity_observer(JNIEnv* env, jobject thiz) {
//	pthread_attr_init(&attr);
//	pthread_create(&thread, &attr, &observer_thread, NULL);

    pid_t pid;
    pid = fork();
    if (pid == -1) {
        __android_log_print(ANDROID_LOG_INFO, TAG, "Fork failed\n");
    } else if (pid == 0) {
        __android_log_print(ANDROID_LOG_INFO, TAG, "Fork child\n");
        observer();
    } else {
        __android_log_print(ANDROID_LOG_INFO, TAG, "Fork parent %d\n", getpid());
    }
}

/*

 char *pathToSo = "/data/data/pl.sdasda/file.so";

 void *handle = dlopen(pathToSo, RTLD_LAZY);
 if (!handle) {
 	// cannot load the plugin; dlerror()
 	return;
 }

 // clear existing error
 dlerror();

 typedef void (*KGEMain) ();

 pFn = (KGEMain) dlsym(handle, "KGEmain");
 if ((error = dlerror()) != NULL || !pFn) {


 }

 pFn();


 */
