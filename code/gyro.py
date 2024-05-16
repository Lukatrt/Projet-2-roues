import smbus
import time

# I2C bus number (might be different on your system)
bus = smbus.SMBus(1)

# I2C addresses
GYRO = 0x68
ACCEL = 0x53

# Gyro registers
REG_GYRO_X = 0x1D
# Other gyro registers can be found in the gyro datasheet

# Accel register (assuming ADXL345)
ADXL345_POWER_CTL = 0x2D

def write_to(device, address, value):
  """Writes a value to a register on an I2C device."""
  bus.write_byte_data(device, address, value)

def setup():
  # Initialize I2C communication
  bus.open()

  # Set Gyro settings
  write_to(GYRO, 0x16, 0x0B)  # Sample Rate 1kHz, Filter Bandwidth 42Hz, Gyro Range 500 d/s
  write_to(GYRO, 0x18, 0x32)  # set accel register data address
  write_to(GYRO, 0x14, ACCEL)  # set accel i2c slave address
  write_to(GYRO, 0x3D, 0x08)  # Set passthrough mode to Accel so we can turn it on
  write_to(ACCEL, ADXL345_POWER_CTL, 8)  # set accel power control to 'measure'
  write_to(GYRO, 0x3D, 0x28)  # Cancel passthrough to accel, gyro will now read accel for us

def loop():
  # Read data from Gyro and Accel
  bus.write_byte(GYRO, REG_GYRO_X)  # Start reading from Gyro X register
  data = bus.read_i2c_block_data(GYRO, 12)  # Read 12 bytes of data

  # Combine bytes into integers
  # Gyro format is MSB first
  gyro_x = (data[0] << 8) | data[1]
  gyro_y = (data[2] << 8) | data[3]
  gyro_z = (data[4] << 8) | data[5]

  # Accel is LSB first. Swap X and Y based on orientation.
  accel_y = (data[7] << 8) | data[6]
  accel_x = (data[9] << 8) | data[8]
  accel_z = (data[11] << 8) | data[10]

  # Print sensor data
  print("Gyro x:", gyro_x, "y:", gyro_y, "z:", gyro_z)
  print("Accel x:", accel_x, "y:", accel_y, "z:", accel_z)

  # Delay between readings
  time.sleep(0.05)  # 50ms delay

if __name__ == "__main__":
  setup()
  while True:
    loop()
