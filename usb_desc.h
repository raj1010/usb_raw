/**
  ******************************************************************************
  * @file    usb_desc.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Descriptors Header for Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

//Standard Descriptors
#define DEVICE_DESC_TYPE                        (0x01)
#define CONFIG_DESC_TYPE                        (0x02)
#define STRING_DESC_TYPE                        (0x03)
#define INTERFACE_DESC_TYPE                     (0x04)
#define ENDPOINT_DESC_TYPE                      (0x05)
#define DEVICE_QUALIFIER_TYPE                   (0x06)
#define OTHER_SPEED_CONFIG                      (0x07)   
#define OTG_DESC_TYPE                           (0x08)

#define HID_DESCRIPTOR_TYPE                     (0x21)
#define REPORT_DESC_TYPE                        (0x22)
#define JOYSTICK_SIZ_HID_DESC                   (0x09)
#define JOYSTICK_OFF_HID_DESC                   (0x12)

#define SIZ_DEVICE_DESC                         (18)
#define SIZ_CONFIG_DESC                         (34)
#define JOYSTICK_SIZ_REPORT_DESC                         (74)
#define SIZ_STRING_LANGID                       (4)
#define SIZ_STRING_VENDOR                       (38)
#define SIZ_STRING_PRODUCT                      (30)
#define SIZ_STRING_SERIAL                       (26)

#define STANDARD_ENDPOINT_DESC_SIZE             (0x09)

/* Exported functions ------------------------------------------------------- */
extern const uint8_t DeviceDescriptor[SIZ_DEVICE_DESC];
extern const uint8_t ConfigDescriptor[SIZ_CONFIG_DESC];
extern const uint8_t Joystick_ReportDescriptor[JOYSTICK_SIZ_REPORT_DESC];
extern const uint8_t StringLangID[SIZ_STRING_LANGID];
extern const uint8_t StringVendor[SIZ_STRING_VENDOR];
extern const uint8_t StringProduct[SIZ_STRING_PRODUCT];
extern uint8_t StringSerial[SIZ_STRING_SERIAL];

#endif /* __USB_DESC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/ 
