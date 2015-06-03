adb -s emulator-5554 install -r ./app/build/outputs/apk/app-debug.apk
adb -s emulator-5554 shell am start -n "pl.pelotasplus.actionafteruninstall/pl.pelotasplus.actionafteruninstall.MainActivity" -a android.intent.action.MAIN -c android.intent.category.LAUNCHER
