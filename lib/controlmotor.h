// code duoc kham khao tu nhieu nguon 
#include <Wire.h> //thư viện I2c của Arduino, do PCA9685 sử dụng chuẩn giao tiếp i2c nên thư viện này bắt buộc phải khai báo 
#include <Adafruit_PWMServoDriver.h> // thư viện PCA9685
#include <PS2X_lib.h> // thư viện điều khiển 


#define ServoDriver_ADDRESS 0x70
//Định nghĩa các chân điều khiển (i2c) 
#define PS2_DAT 12 // MISO 
#define PS2_CMD 13 // MOSI 
#define PS2_SEL 15 // SS 
#define PS2_CLK 14 // SLK

#define Mleft_1  8 //motor trái
#define Mleft_2  9
#define Mright_1  14 //motor phải
#define Mright_2  15

//khac
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(ServoDriver_ADDRESS); //Khởi tạo class PWM của mạch công suất của motor siu
PS2X ps2x; // khởi tạo class PS2x

#define Y_JOY_CALIB 128 
// giá trị của joystick khi không tác động 
#define Max_speed 4095
#define Norm_speed 2048

// set up động cơ- BEGIN
void setPWMMotors(int c1, int c2, int c3, int c4) //set vận tốc 
{
  pwm.setPWM(Mleft_1, 0, c1);
  pwm.setPWM(Mleft_2, 0, c2);
  pwm.setPWM(Mright_1, 0, c3);
  pwm.setPWM(Mright_2, 0, c4);
}

void initMotors() // khai báo động cơ
{
  Wire.begin(); // SDA, SCL,400000);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000); // cài đặt tần số dao động 
  pwm.setPWMFreq(50);  // cài đặt tần số PWM. Tần số PWM có thể được cài đặt trong khoảng 24-1600 HZ, tần số này được cài đặt tùy thuộc vào nhu cầu xử dụng. Để điều khiển được cả servo và động cơ DC cùng nhau, tần số PWM điều khiển được cài đặt trong khoảng 50-60Hz.
  Wire.setClock(400000); // cài đặt tốc độ giao tiếp i2c ở tốc độ cao nhất(400 Mhz). Hàm này có thể bỏ qua nếu gặp lỗi hoặc không có nhu cầu tử dụng I2c tốc độ cao

  setPWMMotors(0, 0, 0, 0);
}
// setup động cơ - END

// set up điều khiển - BEGIN
void setupPS2controller() // set up kết nối lập vô hạn đến khi có kết nối
{
  int err = -1;
  while (err != 0)
  {
    err = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, true, true);
  }

}
/*
* giữ r2 để tăng tốc
*/
bool PS2control()
{
  int speed = Norm_speed;
  if (ps2x.Button(PSB_R2))
    speed = Max_speed;

  int nJoyrY = ps2x.Analog(PSS_RY) - Y_JOY_CALIB; // read y-joystick || driving mode đang ở sing hay two, sing thi - PSS_RY va ngược lại 
  int nJoylY = ps2x.Analog(PSS_LY) - Y_JOY_CALIB;
  int nMotMixL;                          // Motor (left) mixed output
  int nMotMixR;                          // Motor (right) mixed output

  if(nJoylY== 0 && nJoyrY== 0) // in case of lost connection with the wireless controller, only used in VRC2023 PS2 wireless controller 
  {
    setPWMMotors(0, 0, 0, 0);
    return 0;
  }


  int c1 = 0, c2 = 0, c3 = 0, c4 = 0;

    if (nJoyrY > 0)
    {
      c3 = nJoyrY;
      c3 = map(c3, 0, 128, 0, speed);
    }
    else if (nJoyrY < 0)
    {
      c4 = abs(nJoyrY) ; 
      c4 = map(c4, 0, 128, 0, speed);
    }

    if (nJoylY > 0)
    {
      c1 = nJoylY;
      c1 = map(c1, 0, 128, 0, speed);
    }
    else if (nJoylY < 0)
    {
      c2 = abs(nJoylY);
      c2 = map(c2, 0, 128, 0, speed);
    }
    setPWMMotors(c1, c2, c3, c4);
    return 1;

}
// set up dieu khien - END
