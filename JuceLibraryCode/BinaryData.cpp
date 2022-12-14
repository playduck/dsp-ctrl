/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#include <cstring>

namespace BinaryData
{

//================== UsbSerialHelper.java ==================
static const unsigned char temp_binary_data_0[] =
"package com.artiphon.juce_serial;\n"
"\n"
"import android.app.Activity;\n"
"import android.app.PendingIntent;\n"
"import android.content.Context;\n"
"import android.content.Intent;\n"
"import android.hardware.usb.UsbDevice;\n"
"import android.hardware.usb.UsbDeviceConnection;\n"
"import android.hardware.usb.UsbManager;\n"
"import android.os.Handler;\n"
"import android.os.Looper;\n"
"import android.util.Log;\n"
"\n"
"import com.hoho.android.usbserial.driver.CdcAcmSerialDriver;\n"
"import com.hoho.android.usbserial.driver.UsbSerialPort;\n"
"import com.hoho.android.usbserial.util.SerialInputOutputManager;\n"
"\n"
"import java.io.IOException;\n"
"import java.util.ArrayDeque;\n"
"import java.util.Deque;\n"
"import java.util.concurrent.Executors;\n"
"\n"
"import static android.content.Context.USB_SERVICE;\n"
"import java.util.List;\n"
"import java.util.concurrent.locks.ReentrantLock;\n"
"\n"
"public class UsbSerialHelper implements SerialInputOutputManager.Listener {\n"
"\n"
"    private enum UsbPermission { Unknown, Requested, Granted, Denied };\n"
"\n"
"    private static final String INTENT_ACTION_GRANT_USB = \"MY_APP.GRANT_USB\";\n"
"    private static final int WRITE_WAIT_MILLIS = 2000;\n"
"    private static final int READ_WAIT_MILLIS = 2000;\n"
"\n"
"    private int baudRate = -1;\n"
"    private int dataBits = -1;\n"
"    private int stopBits = -1;\n"
"    private int parity = -1;\n"
"\n"
"    private Handler mainLooper;\n"
"\n"
"    private SerialInputOutputManager usbIoManager;\n"
"    private UsbSerialPort usbSerialPort;\n"
"    private UsbPermission usbPermission = UsbPermission.Unknown;\n"
"    private boolean connected = false;\n"
"\n"
"    Context context;\n"
"    Activity mainActivity;\n"
"\n"
"    Deque<Byte> readDeque;\n"
"    ReentrantLock readDequeLock;\n"
"\n"
"    public void DBG(String text, Context context) {\n"
"//        Toast.makeText(context, text, Toast.LENGTH_SHORT).show();\n"
"        Log.d(\"UsbSerialHelper#DBG\", text);\n"
"    }\n"
"\n"
"    String getSerialPortPaths(Context contextIn) {\n"
"        try {\n"
"            UsbManager usbManager = (UsbManager) context.getSystemService (USB_SERVICE);\n"
"\n"
"            for (UsbDevice device : usbManager.getDeviceList().values()) {\n"
"                CdcAcmSerialDriver driver = new CdcAcmSerialDriver(device);\n"
"\n"
"                List<UsbSerialPort> ports = driver.getPorts();\n"
"\n"
"                String allPorts = \"\";\n"
"                for (UsbSerialPort s : ports)\n"
"                    allPorts += \"serialport \" + s.getPortNumber() + \"-\";\n"
"\n"
"                return allPorts;\n"
"            }\n"
"        } catch (Exception e) {\n"
"            e.printStackTrace();\n"
"        }\n"
"\n"
"        return \"\";\n"
"    }\n"
"\n"
"    public UsbSerialHelper(Context contextIn, Activity mainActivityIn) {\n"
"        context = contextIn;\n"
"        mainActivity = mainActivityIn;\n"
"        mainLooper = new Handler(Looper.getMainLooper());\n"
"        readDeque = new ArrayDeque<>(8192);\n"
"        readDequeLock = new ReentrantLock();\n"
"    }\n"
"\n"
"    public boolean connect(int portNum) {\n"
"        UsbDevice device = null;\n"
"        UsbManager usbManager = (UsbManager) context.getSystemService(USB_SERVICE);\n"
"\n"
"        //TODO: deal with the case where there`s more than 1 device\n"
"        for (UsbDevice v : usbManager.getDeviceList().values())\n"
"            //if (v.getDeviceId() == deviceId)\n"
"            device = v;\n"
"\n"
"        if (device == null) {\n"
"            DBG(\"connection failed: device not found\", context);\n"
"            return false;\n"
"        }\n"
"\n"
"        CdcAcmSerialDriver driver = new CdcAcmSerialDriver(device);\n"
"        if (driver.getPorts().size() < portNum) {\n"
"            DBG(\"connection failed: not enough ports at device\", context);\n"
"            return false;\n"
"        }\n"
"\n"
"        usbSerialPort = driver.getPorts().get(portNum);\n"
"\n"
"        UsbDeviceConnection usbConnection = usbManager.openDevice(driver.getDevice());\n"
"        if (usbConnection == null && usbPermission == UsbPermission.Unknown && !usbManager.hasPermission(driver.getDevice())) {\n"
"            usbPermission = UsbPermission.Requested;\n"
"            PendingIntent usbPermissionIntent = PendingIntent.getBroadcast(mainActivity, 0, new Intent(INTENT_ACTION_GRANT_USB), 0);\n"
"            usbManager.requestPermission(driver.getDevice(), usbPermissionIntent);\n"
"            return false;\n"
"        }\n"
"\n"
"        if (usbConnection == null) {\n"
"            if (! usbManager.hasPermission(driver.getDevice()))\n"
"                DBG(\"connection failed: permission denied\", context);\n"
"            else\n"
"                DBG(\"connection failed: open failed\", context);\n"
"            return false;\n"
"        }\n"
"\n"
"        try {\n"
"            usbSerialPort.open(usbConnection);\n"
"            usbIoManager = new SerialInputOutputManager(usbSerialPort, this);\n"
"            Executors.newSingleThreadExecutor().submit(usbIoManager);\n"
"            connected = true;\n"
"            return true;\n"
"        } catch (Exception e) {\n"
"            e.printStackTrace();\n"
"            disconnect();\n"
"            return false;\n"
"        }\n"
"    }\n"
"\n"
"    public boolean setParameters(int baudRateIn, int dataBitsIn, int stopBitsIn, int parityIn) {\n"
"        try {\n"
"            usbSerialPort.setParameters(baudRateIn, dataBitsIn, stopBitsIn, parityIn);\n"
"            baudRate = baudRateIn;\n"
"            dataBits = dataBitsIn;\n"
"            stopBits = stopBitsIn;\n"
"            parity = parityIn;\n"
"//            DBG(\"********************************* UsbSerialHelper#setParameters() baudrate: \" + baudRate + \" dataBits: \" + dataBits + \" stopBits: \" + stopBits + \" parity: \" + parity, context);\n"
"            return true;\n"
"        } catch (IOException e) {\n"
"            e.printStackTrace();\n"
"            baudRate = -1;\n"
"            dataBits = -1;\n"
"            stopBits = -1;\n"
"            parity = -1;\n"
"            return false;\n"
"        }\n"
"    }\n"
"\n"
"    public boolean isOpen() {\n"
"        return usbSerialPort != null && usbSerialPort.isOpen();\n"
"    }\n"
"\n"
"    private void disconnect() {\n"
"        connected = false;\n"
"\n"
"        if (usbIoManager != null)\n"
"            usbIoManager.stop();\n"
"        usbIoManager = null;\n"
"\n"
"        try {\n"
"            if (usbSerialPort != null)\n"
"                usbSerialPort.close();\n"
"        } catch (IOException e) {\n"
"            e.printStackTrace();\n"
"        }\n"
"        usbSerialPort = null;\n"
"    }\n"
"\n"
"    public boolean write (byte[] data) {\n"
"        if (! connected) {\n"
"            DBG(\"not connected\", context);\n"
"            return false;\n"
"        }\n"
"\n"
"        //StringBuilder dbg = new StringBuilder(\"*************** UsbSerialHelper#write ():\");\n"
"        //for (byte b1: data)\n"
"        //    dbg.append(String.format(\"%8s\", Integer.toBinaryString(b1 & 0xFF)).replace(' ', '0') + \" \");\n"
"        //Log.d(\"UsbSerialHelper#write\", dbg.toString());\n"
"\n"
"        try {\n"
"            usbSerialPort.write(data, WRITE_WAIT_MILLIS);\n"
"        } catch (Exception e) {\n"
"            onRunError(e);\n"
"            return false;\n"
"        }\n"
"\n"
"        return true;\n"
"    }\n"
"\n"
"    @Override\n"
"    public void onRunError(Exception e) {\n"
"        mainLooper.post(() -> {\n"
"            DBG(\"connection lost: \" + e.getMessage(), context);\n"
"            disconnect();\n"
"        });\n"
"    }\n"
"\n"
"    //TODO: should this be coalesced in another thread, maybe using the mainLooper, in case we get another call to this callback while we're copying bytes?\n"
"    @Override\n"
"    public void onNewData(byte[] data) {\n"
"        try {\n"
"            readDequeLock.lock();\n"
"\n"
"            //StringBuilder dbg = new StringBuilder(\"*************** UsbSerialHelper#onNewData ():\");\n"
"            //for (byte b1 : data)\n"
"            //    dbg.append(String.format(\"%8s\", Integer.toBinaryString(b1 & 0xFF)).replace(' ', '0') + \" \");\n"
"            //Log.d(\"UsbSerialHelper#onNewDa\", dbg.toString());\n"
"\n"
"            for (byte b : data)\n"
"                readDeque.add(b);\n"
"\n"
"        } catch (Exception e) {\n"
"            e.printStackTrace();\n"
"        } finally {\n"
"            readDequeLock.unlock();\n"
"        }\n"
"    }\n"
"\n"
"    public int read(byte[] buffer) {\n"
"        if (! connected) {\n"
"            //DBG(\"not connected\", context);\n"
"            return 0;\n"
"        }\n"
"\n"
"        int ret = 0;\n"
"\n"
"        try {\n"
"            readDequeLock.lock();\n"
"            int bufferedSize = readDeque.size();\n"
"\n"
"            if (bufferedSize > 0) {\n"
"                //StringBuilder dbg = new StringBuilder(\"*************** UsbSerialHelper#read ():\");\n"
"                for (int i = 0; i < bufferedSize; ++i) {\n"
"                    buffer[i] = readDeque.pop().byteValue();\n"
"                    //dbg.append(String.format(\"%8s\", Integer.toBinaryString(buffer[i] & 0xFF)).replace(' ', '0') + \" \");\n"
"                }\n"
"                //Log.d(\"UsbSerialHelper#read\", dbg.toString());\n"
"\n"
"                ret = bufferedSize;\n"
"            }\n"
"        } catch (Exception e) {\n"
"            DBG(\"**************** issue reading de_que: \" + e.getMessage(), context);\n"
"        } finally {\n"
"            readDequeLock.unlock();\n"
"        }\n"
"\n"
"        return ret;\n"
"    }\n"
"}\n";

const char* UsbSerialHelper_java = (const char*) temp_binary_data_0;

//================== README.md ==================
static const unsigned char temp_binary_data_1[] =
"# juce_serialport\n"
"Access hardware serial ports or USB virtual serial ports\n"
"\n"
"I did not write these classes, but I have been trying to maintain them, since I continue to work on things that need access to serial ports. The only history I have for them is the following:\n"
"\n"
"contributed by graffiti\n"
"\n"
"Updated for current Juce API 8/1/12 Marc Lindahl\n";

const char* README_md = (const char*) temp_binary_data_1;

//================== README.md ==================
static const unsigned char temp_binary_data_2[] =
"#  dsp-ctrl\n"
"\n"
"A GUI for controlling a hardware DSP device.\n"
"\n"
"## Building\n"
"\n"
"This project uses juce 7 with C++14.\n"
"Compile using either MSVC, XCode or Makefile.\n"
"Modifying the project requires projucer.\n"
"\n"
"## References\n"
"\n"
"- This project takes inspiration and partial code from the [Freequalizer Project](https://github.com/ffAudio/Frequalizer).\n"
"- IIR formulas are taken from [robert bristow-johnson's Audio EQ Cookbook](https://webaudio.github.io/Audio-EQ-Cookbook/Audio-EQ-Cookbook.txt).\n"
"- A numerically stable mathematical solution for determining IIR magnitude stems from [robert bristow-johnson's answer on the DSP Stackexchange](https://dsp.stackexchange.com/a/16911/56072). \n"
"- This uses the [juce serialport](https://github.com/cpr2323/juce_serialport) module as a dependency. \n";

const char* README_md2 = (const char*) temp_binary_data_2;


const char* getNamedResource (const char* resourceNameUTF8, int& numBytes);
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes)
{
    unsigned int hash = 0;

    if (resourceNameUTF8 != nullptr)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0xdf7b007b:  numBytes = 8562; return UsbSerialHelper_java;
        case 0x64791dc8:  numBytes = 343; return README_md;
        case 0x2aaa9b6a:  numBytes = 774; return README_md2;
        default: break;
    }

    numBytes = 0;
    return nullptr;
}

const char* namedResourceList[] =
{
    "UsbSerialHelper_java",
    "README_md",
    "README_md2"
};

const char* originalFilenames[] =
{
    "UsbSerialHelper.java",
    "README.md",
    "README.md"
};

const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8)
{
    for (unsigned int i = 0; i < (sizeof (namedResourceList) / sizeof (namedResourceList[0])); ++i)
        if (strcmp (namedResourceList[i], resourceNameUTF8) == 0)
            return originalFilenames[i];

    return nullptr;
}

}
