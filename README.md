# -Calidad-del-Aire-Avanzado
# Monitor de Calidad del Aire Avanzado con Integración de MySQL

## Descripción General
Este proyecto desarrolla un monitor de calidad del aire que detecta formaldehído, temperatura y humedad. Está diseñado para ser utilizado en entornos donde la calidad del aire es crítica, como hogares, oficinas o áreas industriales. Además, el sistema se ha modificado para enviar datos a un servidor MySQL, aumentando así su utilidad para aplicaciones de monitoreo remoto.

## Componentes y Tecnologías
- **ESP32**: Utilizado como controlador principal para gestionar la comunicación y el procesamiento de datos.
- **Módulo GSM SIM7600**: Permite la comunicación GPRS para enviar datos a un servidor remoto.
- **Sensores de Calidad del Aire**: Detectan formaldehído, temperatura y humedad.
- **Pantalla OLED**: Muestra información relevante en tiempo real.
- **Tarjeta SD**: Almacena los registros de datos localmente.
- **NeoPixel LED**: Proporciona retroalimentación visual sobre el estado del sistema.
- **RTC DS3231**: Mantiene la hora exacta para los registros de datos.
- **SoftwareSerial**: Facilita la comunicación con dispositivos externos.

## Instalación y Uso
1. **Montaje del Hardware**: Ensamblando todos los componentes según las especificaciones de diseño, conectando sensores al ESP32.
2. **Configuración del Software**: Cargar el firmware al ESP32, configurar el módulo GSM con los datos APN del proveedor de servicios y establecer la conexión con la red.
3. **Inicialización de Dispositivos**: Verificar la operación correcta del RTC y la tarjeta SD.
4. **Operación y Monitoreo**: Utilizar la pantalla OLED para monitorizar los datos en tiempo real y asegurarse de que los datos se envían correctamente al servidor MySQL.

## Ejemplos de Uso
- **Control de Calidad del Aire en el Hogar**: Monitorear la presencia de contaminantes para garantizar un ambiente seguro en el hogar.
- **Monitoreo Industrial**: Utilizado en fábricas para detectar la presencia de compuestos nocivos y gestionar la calidad del aire.
- **Investigación Ambiental**: Recoger datos a largo plazo sobre la calidad del aire para estudios ambientales.

## Contribuciones y Desarrollo Futuro
- **Integración de Sensores Adicionales**: Ampliación del sistema para incluir sensores de CO2 y otros gases tóxicos.
- **Mejoras de Seguridad**: Implementación de autenticación y encriptación en la comunicación con el servidor MySQL.
- **Desarrollo de Interfaz de Usuario**: Crear una aplicación móvil o web para visualizar y gestionar los datos de manera más eficiente.
"""
