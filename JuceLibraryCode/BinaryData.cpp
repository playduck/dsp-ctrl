/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#include <cstring>

namespace BinaryData
{

//================== UsbSerialHelper.java ==================
static const unsigned char temp_binary_data_0[] =
"package com.artiphon.juce_serial;\r\n"
"\r\n"
"import android.app.Activity;\r\n"
"import android.app.PendingIntent;\r\n"
"import android.content.Context;\r\n"
"import android.content.Intent;\r\n"
"import android.hardware.usb.UsbDevice;\r\n"
"import android.hardware.usb.UsbDeviceConnection;\r\n"
"import android.hardware.usb.UsbManager;\r\n"
"import android.os.Handler;\r\n"
"import android.os.Looper;\r\n"
"import android.util.Log;\r\n"
"\r\n"
"import com.hoho.android.usbserial.driver.CdcAcmSerialDriver;\r\n"
"import com.hoho.android.usbserial.driver.UsbSerialPort;\r\n"
"import com.hoho.android.usbserial.util.SerialInputOutputManager;\r\n"
"\r\n"
"import java.io.IOException;\r\n"
"import java.util.ArrayDeque;\r\n"
"import java.util.Deque;\r\n"
"import java.util.concurrent.Executors;\r\n"
"\r\n"
"import static android.content.Context.USB_SERVICE;\r\n"
"import java.util.List;\r\n"
"import java.util.concurrent.locks.ReentrantLock;\r\n"
"\r\n"
"public class UsbSerialHelper implements SerialInputOutputManager.Listener {\r\n"
"\r\n"
"    private enum UsbPermission { Unknown, Requested, Granted, Denied };\r\n"
"\r\n"
"    private static final String INTENT_ACTION_GRANT_USB = \"MY_APP.GRANT_USB\";\r\n"
"    private static final int WRITE_WAIT_MILLIS = 2000;\r\n"
"    private static final int READ_WAIT_MILLIS = 2000;\r\n"
"\r\n"
"    private int baudRate = -1;\r\n"
"    private int dataBits = -1;\r\n"
"    private int stopBits = -1;\r\n"
"    private int parity = -1;\r\n"
"\r\n"
"    private Handler mainLooper;\r\n"
"\r\n"
"    private SerialInputOutputManager usbIoManager;\r\n"
"    private UsbSerialPort usbSerialPort;\r\n"
"    private UsbPermission usbPermission = UsbPermission.Unknown;\r\n"
"    private boolean connected = false;\r\n"
"\r\n"
"    Context context;\r\n"
"    Activity mainActivity;\r\n"
"\r\n"
"    Deque<Byte> readDeque;\r\n"
"    ReentrantLock readDequeLock;\r\n"
"\r\n"
"    public void DBG(String text, Context context) {\r\n"
"//        Toast.makeText(context, text, Toast.LENGTH_SHORT).show();\r\n"
"        Log.d(\"UsbSerialHelper#DBG\", text);\r\n"
"    }\r\n"
"\r\n"
"    String getSerialPortPaths(Context contextIn) {\r\n"
"        try {\r\n"
"            UsbManager usbManager = (UsbManager) context.getSystemService (USB_SERVICE);\r\n"
"\r\n"
"            for (UsbDevice device : usbManager.getDeviceList().values()) {\r\n"
"                CdcAcmSerialDriver driver = new CdcAcmSerialDriver(device);\r\n"
"\r\n"
"                List<UsbSerialPort> ports = driver.getPorts();\r\n"
"\r\n"
"                String allPorts = \"\";\r\n"
"                for (UsbSerialPort s : ports)\r\n"
"                    allPorts += \"serialport \" + s.getPortNumber() + \"-\";\r\n"
"\r\n"
"                return allPorts;\r\n"
"            }\r\n"
"        } catch (Exception e) {\r\n"
"            e.printStackTrace();\r\n"
"        }\r\n"
"\r\n"
"        return \"\";\r\n"
"    }\r\n"
"\r\n"
"    public UsbSerialHelper(Context contextIn, Activity mainActivityIn) {\r\n"
"        context = contextIn;\r\n"
"        mainActivity = mainActivityIn;\r\n"
"        mainLooper = new Handler(Looper.getMainLooper());\r\n"
"        readDeque = new ArrayDeque<>(8192);\r\n"
"        readDequeLock = new ReentrantLock();\r\n"
"    }\r\n"
"\r\n"
"    public boolean connect(int portNum) {\r\n"
"        UsbDevice device = null;\r\n"
"        UsbManager usbManager = (UsbManager) context.getSystemService(USB_SERVICE);\r\n"
"\r\n"
"        //TODO: deal with the case where there`s more than 1 device\r\n"
"        for (UsbDevice v : usbManager.getDeviceList().values())\r\n"
"            //if (v.getDeviceId() == deviceId)\r\n"
"            device = v;\r\n"
"\r\n"
"        if (device == null) {\r\n"
"            DBG(\"connection failed: device not found\", context);\r\n"
"            return false;\r\n"
"        }\r\n"
"\r\n"
"        CdcAcmSerialDriver driver = new CdcAcmSerialDriver(device);\r\n"
"        if (driver.getPorts().size() < portNum) {\r\n"
"            DBG(\"connection failed: not enough ports at device\", context);\r\n"
"            return false;\r\n"
"        }\r\n"
"\r\n"
"        usbSerialPort = driver.getPorts().get(portNum);\r\n"
"\r\n"
"        UsbDeviceConnection usbConnection = usbManager.openDevice(driver.getDevice());\r\n"
"        if (usbConnection == null && usbPermission == UsbPermission.Unknown && !usbManager.hasPermission(driver.getDevice())) {\r\n"
"            usbPermission = UsbPermission.Requested;\r\n"
"            PendingIntent usbPermissionIntent = PendingIntent.getBroadcast(mainActivity, 0, new Intent(INTENT_ACTION_GRANT_USB), 0);\r\n"
"            usbManager.requestPermission(driver.getDevice(), usbPermissionIntent);\r\n"
"            return false;\r\n"
"        }\r\n"
"\r\n"
"        if (usbConnection == null) {\r\n"
"            if (! usbManager.hasPermission(driver.getDevice()))\r\n"
"                DBG(\"connection failed: permission denied\", context);\r\n"
"            else\r\n"
"                DBG(\"connection failed: open failed\", context);\r\n"
"            return false;\r\n"
"        }\r\n"
"\r\n"
"        try {\r\n"
"            usbSerialPort.open(usbConnection);\r\n"
"            usbIoManager = new SerialInputOutputManager(usbSerialPort, this);\r\n"
"            Executors.newSingleThreadExecutor().submit(usbIoManager);\r\n"
"            connected = true;\r\n"
"            return true;\r\n"
"        } catch (Exception e) {\r\n"
"            e.printStackTrace();\r\n"
"            disconnect();\r\n"
"            return false;\r\n"
"        }\r\n"
"    }\r\n"
"\r\n"
"    public boolean setParameters(int baudRateIn, int dataBitsIn, int stopBitsIn, int parityIn) {\r\n"
"        try {\r\n"
"            usbSerialPort.setParameters(baudRateIn, dataBitsIn, stopBitsIn, parityIn);\r\n"
"            baudRate = baudRateIn;\r\n"
"            dataBits = dataBitsIn;\r\n"
"            stopBits = stopBitsIn;\r\n"
"            parity = parityIn;\r\n"
"//            DBG(\"********************************* UsbSerialHelper#setParameters() baudrate: \" + baudRate + \" dataBits: \" + dataBits + \" stopBits: \" + stopBits + \" parity: \" + parity, context);\r\n"
"            return true;\r\n"
"        } catch (IOException e) {\r\n"
"            e.printStackTrace();\r\n"
"            baudRate = -1;\r\n"
"            dataBits = -1;\r\n"
"            stopBits = -1;\r\n"
"            parity = -1;\r\n"
"            return false;\r\n"
"        }\r\n"
"    }\r\n"
"\r\n"
"    public boolean isOpen() {\r\n"
"        return usbSerialPort != null && usbSerialPort.isOpen();\r\n"
"    }\r\n"
"\r\n"
"    private void disconnect() {\r\n"
"        connected = false;\r\n"
"\r\n"
"        if (usbIoManager != null)\r\n"
"            usbIoManager.stop();\r\n"
"        usbIoManager = null;\r\n"
"\r\n"
"        try {\r\n"
"            if (usbSerialPort != null)\r\n"
"                usbSerialPort.close();\r\n"
"        } catch (IOException e) {\r\n"
"            e.printStackTrace();\r\n"
"        }\r\n"
"        usbSerialPort = null;\r\n"
"    }\r\n"
"\r\n"
"    public boolean write (byte[] data) {\r\n"
"        if (! connected) {\r\n"
"            DBG(\"not connected\", context);\r\n"
"            return false;\r\n"
"        }\r\n"
"\r\n"
"        //StringBuilder dbg = new StringBuilder(\"*************** UsbSerialHelper#write ():\");\r\n"
"        //for (byte b1: data)\r\n"
"        //    dbg.append(String.format(\"%8s\", Integer.toBinaryString(b1 & 0xFF)).replace(' ', '0') + \" \");\r\n"
"        //Log.d(\"UsbSerialHelper#write\", dbg.toString());\r\n"
"\r\n"
"        try {\r\n"
"            usbSerialPort.write(data, WRITE_WAIT_MILLIS);\r\n"
"        } catch (Exception e) {\r\n"
"            onRunError(e);\r\n"
"            return false;\r\n"
"        }\r\n"
"\r\n"
"        return true;\r\n"
"    }\r\n"
"\r\n"
"    @Override\r\n"
"    public void onRunError(Exception e) {\r\n"
"        mainLooper.post(() -> {\r\n"
"            DBG(\"connection lost: \" + e.getMessage(), context);\r\n"
"            disconnect();\r\n"
"        });\r\n"
"    }\r\n"
"\r\n"
"    //TODO: should this be coalesced in another thread, maybe using the mainLooper, in case we get another call to this callback while we're copying bytes?\r\n"
"    @Override\r\n"
"    public void onNewData(byte[] data) {\r\n"
"        try {\r\n"
"            readDequeLock.lock();\r\n"
"\r\n"
"            //StringBuilder dbg = new StringBuilder(\"*************** UsbSerialHelper#onNewData ():\");\r\n"
"            //for (byte b1 : data)\r\n"
"            //    dbg.append(String.format(\"%8s\", Integer.toBinaryString(b1 & 0xFF)).replace(' ', '0') + \" \");\r\n"
"            //Log.d(\"UsbSerialHelper#onNewDa\", dbg.toString());\r\n"
"\r\n"
"            for (byte b : data)\r\n"
"                readDeque.add(b);\r\n"
"\r\n"
"        } catch (Exception e) {\r\n"
"            e.printStackTrace();\r\n"
"        } finally {\r\n"
"            readDequeLock.unlock();\r\n"
"        }\r\n"
"    }\r\n"
"\r\n"
"    public int read(byte[] buffer) {\r\n"
"        if (! connected) {\r\n"
"            //DBG(\"not connected\", context);\r\n"
"            return 0;\r\n"
"        }\r\n"
"\r\n"
"        int ret = 0;\r\n"
"\r\n"
"        try {\r\n"
"            readDequeLock.lock();\r\n"
"            int bufferedSize = readDeque.size();\r\n"
"\r\n"
"            if (bufferedSize > 0) {\r\n"
"                //StringBuilder dbg = new StringBuilder(\"*************** UsbSerialHelper#read ():\");\r\n"
"                for (int i = 0; i < bufferedSize; ++i) {\r\n"
"                    buffer[i] = readDeque.pop().byteValue();\r\n"
"                    //dbg.append(String.format(\"%8s\", Integer.toBinaryString(buffer[i] & 0xFF)).replace(' ', '0') + \" \");\r\n"
"                }\r\n"
"                //Log.d(\"UsbSerialHelper#read\", dbg.toString());\r\n"
"\r\n"
"                ret = bufferedSize;\r\n"
"            }\r\n"
"        } catch (Exception e) {\r\n"
"            DBG(\"**************** issue reading de_que: \" + e.getMessage(), context);\r\n"
"        } finally {\r\n"
"            readDequeLock.unlock();\r\n"
"        }\r\n"
"\r\n"
"        return ret;\r\n"
"    }\r\n"
"}\r\n";

const char* UsbSerialHelper_java = (const char*) temp_binary_data_0;

//================== README.md ==================
static const unsigned char temp_binary_data_1[] =
"# juce_serialport\r\n"
"Access hardware serial ports or USB virtual serial ports\r\n"
"\r\n"
"I did not write these classes, but I have been trying to maintain them, since I continue to work on things that need access to serial ports. The only history I have for them is the following:\r\n"
"\r\n"
"contributed by graffiti\r\n"
"\r\n"
"Updated for current Juce API 8/1/12 Marc Lindahl\r\n";

const char* README_md = (const char*) temp_binary_data_1;

//================== README.md ==================
static const unsigned char temp_binary_data_2[] =
"#  dsp-ctrl\r\n"
"\r\n"
"A GUI for controlling a hardware DSP device.\r\n"
"\r\n"
"## Building\r\n"
"\r\n"
"This project uses juce 7 with C++14.\r\n"
"Compile using either MSVC, XCode or Makefile.\r\n"
"Modifying the project requires projucer.\r\n"
"\r\n"
"## References\r\n"
"\r\n"
"- This project takes inspiration and partial code from the [Freequalizer Project](https://github.com/ffAudio/Frequalizer).\r\n"
"- IIR formulas are taken from [robert bristow-johnson's Audio EQ Cookbook](https://webaudio.github.io/Audio-EQ-Cookbook/Audio-EQ-Cookbook.txt).\r\n"
"- A numerically stable mathematical solution for determining IIR magnitude stems from [robert bristow-johnson's answer on the DSP Stackexchange](https://dsp.stackexchange.com/a/16911/56072). \r\n"
"- This uses the [juce serialport](https://github.com/cpr2323/juce_serialport) module as a dependency. \r\n";

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
        case 0xdf7b007b:  numBytes = 8820; return UsbSerialHelper_java;
        case 0x64791dc8:  numBytes = 351; return README_md;
        case 0x2aaa9b6a:  numBytes = 790; return README_md2;
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
