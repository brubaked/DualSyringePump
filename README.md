# DualSyringePump
Arduino code to control a dual syringe pump with arbitrary syringe sizes and flow rates. 

Syringe pumps from SyringePumps.com (http://www.syringepump.com/oem.php#aa, NE-500) were used, though most any stepper motor controlled set of syringe pumps would work. 

An ITEAD dual stepper motor control shield (https://www.itead.cc/wiki/Arduino_Dual_Step_Motor_Driver_Shield) was used along with an Arduino UNO to control the whole shebang. 

At the moment, flow rates are controlled by editing the variable values in the source code, and reuploading the code to the Arduino, but serial control could be easilly added. 
