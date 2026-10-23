# TP02 — Control y monitoreo de ESP32 mediante Telegram

**Materia:** Tecnologías para la Automatización  
**Institución:** UTN — Facultad Regional Venado Tuerto  
**Integrantes:** Tomás Acosta, Donato Cacchiarelli e Iván Salomón

## Descripción

Este trabajo práctico implementa un sistema de control y monitoreo sobre una **ESP32**. Desde un bot de **Telegram** se pueden controlar dos LEDs, consultar un sensor de temperatura y humedad DHT22, leer el voltaje de un potenciómetro, mostrar información en un display OLED y enviar mediciones a **ThingSpeak** para visualizarlas en un dashboard.

El firmware se desarrolla con **Arduino Framework y PlatformIO** y se ejecuta en el simulador **Wokwi**. Los comandos se reciben a través de la red WiFi simulada y se procesan en módulos propios.

## Componentes y conexiones

| Componente | Conexión a ESP32 | Función |
|---|---|---|
| LED verde + resistencia de 330 Ω | GPIO 23 | Encendido y apagado remoto |
| LED azul + resistencia de 330 Ω | GPIO 2 | Encendido y apagado remoto |
| DHT22 | GPIO 15 | Temperatura y humedad |
| Potenciómetro | GPIO 34 (ADC) | Lectura analógica y voltaje aproximado de 0 a 3,3 V |
| OLED SSD1306 128×64 | SDA: GPIO 21; SCL: GPIO 22 | Visualización de estados y mediciones |

Los componentes se alimentan desde 3,3 V y GND según el circuito representado en `diagram.json`. El monitor serie de Wokwi está conectado al UART de la ESP32.

## Comandos disponibles

| Comando | Acción |
|---|---|
| `/start` | Muestra la bienvenida y la lista de comandos disponibles |
| `/led23on` / `/led23off` | Enciende / apaga el LED verde (GPIO 23) |
| `/led2on` / `/led2off` | Enciende / apaga el LED azul (GPIO 2) |
| `/dht22` | Informa temperatura (°C) y humedad relativa (%) |
| `/pote` | Informa el voltaje aproximado del potenciómetro (V) |
| `/platiot` | Envía temperatura y humedad a ThingSpeak e informa el resultado |
| `/displayled` | Muestra el estado de ambos LEDs en el OLED |
| `/displaydht22` | Muestra temperatura y humedad en el OLED |
| `/displaypote` | Muestra el voltaje del potenciómetro en el OLED |

Los comandos desconocidos generan una respuesta de error. Si el comando comienza con `/display` pero no corresponde a un subcomando válido, el OLED también informa el error.

## Arquitectura del software

La lógica se distribuye en módulos dentro de `lib/`, para evitar concentrarla en un único sketch:

```text
src/
└── main.cpp                  # Inicializa y coordina los módulos

include/
├── AppConfig.h              # Pines y configuración general
├── Secrets.example.h        # Plantilla para configurar credenciales
└── Secrets.h                # Credenciales locales (no versionadas)

lib/
├── Hardware/
│   ├── Hardware.h
│   └── Hardware.cpp          # DHT22, ADC y LEDs
├── Display/
│   ├── Display.h
│   └── Display.cpp           # OLED SSD1306
├── WifiService/
│   ├── WifiService.h
│   └── WifiService.cpp       # Conexión y reconexión WiFi
├── Telegram/
│   ├── TelegramBot.h
│   └── TelegramBot.cpp       # Recepción, interpretación y respuestas
└── ThingSpeak/
    ├── ThingSpeakClient.h
    └── ThingSpeakClient.cpp  # Publicación en la plataforma IoT

platformio.ini                # Entorno y dependencias
wokwi.toml                   # Ubicación del firmware para Wokwi
diagram.json                 # Circuito simulado
```

**Flujo general:**

1. `main.cpp` inicializa hardware, display, WiFi y bot de Telegram.
2. `WifiService` gestiona la conectividad de la ESP32.
3. `TelegramBot` consulta mensajes mediante la API de Telegram e identifica el comando recibido.
4. Según el comando, invoca `Hardware`, `Display` o `ThingSpeakClient`.
5. Devuelve una respuesta al chat y, cuando corresponde, registra la operación en el monitor serie.

El procesamiento de mensajes funciona mediante consultas periódicas (*polling*). La supervisión y los reintentos de WiFi se realizan con `millis()`, sin un bucle de espera indefinido en la inicialización.

## Tecnologías y dependencias

- **ESP32 / Arduino Framework:** ejecución del firmware.
- **PlatformIO IDE/Core:** gestión del proyecto, dependencias y compilación.
- **Wokwi:** simulación del circuito y de la red de la ESP32.
- **UniversalTelegramBot:** intercambio de mensajes con la API de Telegram.
- **ArduinoJson:** procesamiento JSON utilizado por la integración de Telegram.
- **DHT sensor library for ESPx:** lectura del sensor DHT22.
- **Adafruit GFX / Adafruit SSD1306:** visualización en el OLED.
- **ThingSpeak:** recepción y visualización de las mediciones del DHT22.

Las versiones de las librerías están definidas en `platformio.ini`.

## Configuración y ejecución

### 1. Clonar el proyecto

```bash
git clone https://github.com/NiFoox/tpa-frvt-tp02-26.git
cd tpa-frvt-tp02-26
```

Abrir la carpeta del proyecto en VS Code con las extensiones **PlatformIO IDE** y **Wokwi Simulator** instaladas. Para simular localmente se necesita una licencia de Wokwi válida.

### 2. Configurar credenciales locales

Copiar `include/Secrets.example.h` a `include/Secrets.h` y completar **localmente**:

- SSID y contraseña de WiFi (en la simulación: `Wokwi-GUEST`, sin contraseña).
- Token vigente del bot de Telegram, generado mediante [@BotFather](https://t.me/BotFather).
- Chat ID autorizado, si se decide limitar el acceso (cadena vacía habilita a cualquier chat en la implementación actual).
- **Write API Key** del canal de ThingSpeak.

`include/Secrets.h` está excluido mediante `.gitignore`: **no subir tokens ni claves a GitHub**. No publicar tampoco esos valores en capturas, issues o logs. Si una credencial se expone, debe revocarse y regenerarse.

### 3. Compilar

Desde la terminal integrada de VS Code:

```bash
pio run
```

Se generan, entre otros, los archivos:

```text
.pio/build/esp32doit-devkit-v1/firmware.bin
.pio/build/esp32doit-devkit-v1/firmware.elf
```

### 4. Ejecutar Wokwi

Abrir `diagram.json` y ejecutar el simulador con el botón de inicio, o usar el comando **Wokwi: Start Simulator** desde la paleta de comandos. `wokwi.toml` contiene las rutas del firmware generado.

Recompilar con `pio run` después de modificar el código, antes de reiniciar la simulación.

### 5. Probar el bot

Abrir el bot previamente configurado en Telegram, enviar `/start` y probar los comandos de la tabla anterior. El monitor serie de Wokwi muestra eventos y diagnósticos.

## ThingSpeak

El comando `/platiot` consulta el DHT22 y, si la lectura es válida, publica:

- **Field 1:** temperatura (°C).
- **Field 2:** humedad relativa (%).

`ThingSpeakClient` utiliza el endpoint `/update` y verifica la respuesta HTTP y el identificador de la entrada creada. Para demostrar el resultado, abrir el dashboard del canal de ThingSpeak y comparar las mediciones con las mostradas por Telegram y Serial.

> Enlace al canal/dashboard: **agregar aquí la URL compartible del canal**, sin incluir la Write API Key.

## Verificaciones para la demostración

- Compilación exitosa mediante PlatformIO.
- Encendido y apagado independiente de ambos LEDs desde Telegram.
- Lecturas de DHT22 y potenciómetro coherentes con la simulación.
- Visualización en OLED y aviso de subcomando desconocido.
- Envío de temperatura y humedad a ThingSpeak y comprobación en el dashboard.
- Participación de los tres integrantes en la explicación y demostración.

## Alcance y limitaciones actuales

Este repositorio corresponde a un **prototipo académico**. La versión trabajada permite el uso de Chat ID vacío para facilitar una demostración grupal; en ese modo, cualquier persona que encuentre el bot puede enviarle comandos mientras esté activo.

La integración de Telegram utiliza temporalmente `setInsecure()` (sin verificación del certificado del servidor) y el envío actual a ThingSpeak emplea HTTP sin cifrar. Estas decisiones son **limitaciones de seguridad pendientes de revisión**, no características recomendadas para un despliegue real. Las credenciales publicadas en commits anteriores deben estar revocadas; moverlas a `Secrets.h` no elimina automáticamente su exposición histórica.

No se incluyen pruebas automatizadas ni se afirma compatibilidad con hardware físico sin su correspondiente validación.
