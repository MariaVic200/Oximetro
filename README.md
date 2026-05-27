# Oximetro (Arduino)

Guía paso a paso para descargar, abrir y cargar el código en tu Arduino por primera vez.

> Este proyecto está pensado para principiantes y asume que nunca has usado Git/GitHub.

---

## 1) Programas necesarios

### 1.1 Git
Git permite descargar el proyecto desde GitHub y actualizarlo cuando haya cambios.

1. Descarga Git:
   https://git-scm.com/downloads
2. Instálalo.
3. Abre **Git Bash** y verifica que funcione con:

```txt
git --version
```

Si aparece una versión (ejemplo: `git version ...`), la instalación fue correcta.

---

### 1.2 Arduino IDE
Arduino IDE permite abrir, editar y cargar el código al Arduino.

Descargar:
https://www.arduino.cc/en/software

---

### 1.3 Visual Studio Code (opcional)
Es opcional, pero recomendado para revisar archivos y usar Git de forma más cómoda.

Descargar:
https://code.visualstudio.com/

---

## 2) Descargar el proyecto desde GitHub

1. Abre **Git Bash**.

2. Entra a la carpeta donde quieres guardar el proyecto (ruta típica en Windows). Por ejemplo, el escritorio:

```txt
cd /c/Users/TU_USUARIO/Desktop
```

> Reemplaza `TU_USUARIO` por tu nombre de usuario de Windows.

3. Descarga el repositorio con:

```txt
git clone https://github.com/MariaVic200/Oximetro.git
```


Esto creará una carpeta llamada:

- `Oximetro`

4. Entra a la carpeta del proyecto:

```txt
cd Oximetro
```

5. Verifica que estás dentro del repositorio con:

```txt
git status
```

Debe aparecer algo similar a:

- `On branch main`
- `nothing to commit, working tree clean`

---

## 3) Abrir el proyecto en Arduino IDE

1. Abre **Arduino IDE**.
2. Ve a:
   **File → Open**
3. Busca la carpeta donde descargaste el proyecto y abre la carpeta `Oximetro`.

4. El archivo principal es un `.ino`. En este repo hay **varias versiones** dentro de la carpeta `Arduino/`. Una buena opción para empezar es:

- `Arduino/v4/v4.ino`

> Si en algún momento no te carga o te da errores, puedes probar con otra versión dentro de `Arduino/`.

---

## 4) Seleccionar la placa en Arduino IDE

Antes de cargar el código, selecciona la placa correcta.

- En Arduino IDE ve a:
  **Tools → Board**
- Selecciona tu placa, por ejemplo:
  - **Arduino Uno**
  - u otra que tengas

---

## 5) Seleccionar el puerto (COM)

1. Conecta el Arduino al computador por USB.
2. En Arduino IDE ve a:
   **Tools → Port**
3. Selecciona el puerto que aparezca, por ejemplo:

- `COM3`
- `COM4`
- `COM5`

> El número puede variar según tu PC.

---

## 6) Verificar (compilar) el código

Para comprobar que el código no tenga errores:

- Presiona el botón **✔ (Verify/Compile)**

También puedes usar:

- **Sketch → Verify/Compile**

Si no aparecen errores, el código está listo para cargarse.

---

## 7) Cargar el código al Arduino

1. Presiona el botón **→ (Upload)**

También puedes usar:

- **Sketch → Upload**

Cuando termine correctamente, Arduino IDE mostrará un mensaje indicando que la carga fue completada.

---

## 8) Actualizar el proyecto cuando haya cambios en GitHub

Si otra persona sube cambios al repositorio, debes actualizar tu carpeta local.

1. Abre Git Bash.
2. Entra a la carpeta del proyecto:

```txt
cd /ruta/del/proyecto/Oximetro
```


3. Ejecuta:

```txt
git pull
```

Esto descarga los cambios más recientes desde GitHub.

---

## 9) Guardar cambios propios en Git

Cuando modifiques el código en Arduino IDE y quieras subirlo a GitHub:

1. Guarda el archivo en Arduino IDE con:
   - **Ctrl + S**

2. Abre Git Bash y entra a la carpeta del proyecto (si no estás ahí):

```bash
cd /ruta/del/proyecto/Oximetro
```


3. Revisa qué archivos cambiaron:

```txt
git status
```

4. Agrega los cambios:

```txt
git add .
```

5. Crea un commit con un mensaje descriptivo:

```txt
git commit -m "Actualizo codigo del oximetro"
```

6. Sube tus cambios a GitHub:

```txt
git push
```

---

## 10) Comandos básicos de Git (explicación simple)

### Ver el estado del proyecto
```txt
git status
```

Sirve para saber si hay cambios pendientes.

### Descargar cambios desde GitHub
```txt
git pull
```

Sirve para actualizar tu carpeta local con lo más reciente.

### Agregar cambios
```txt
git add .
```

Prepara los archivos modificados para guardarlos en Git.

### Crear un commit
```txt
git commit -m "Mensaje del cambio"
```

Guarda una “versión” del proyecto con una descripción.

### Subir cambios a GitHub
```txt
git push
```

Envía tus cambios al repositorio remoto en GitHub.

---

## 11) Recomendaciones importantes

- Antes de comenzar a trabajar, ejecuta siempre:

```txt
git pull
```

Esto evita trabajar sobre una versión desactualizada.

- Después de modificar el código, guarda en Arduino IDE con:
  **Ctrl + S**

- Luego sube tus cambios con:

```txt
git add .
git commit -m "Descripcion del cambio realizado"
git push
```

- No borres archivos del proyecto si no estás seguro de su función.
- No cambies el nombre de la carpeta principal del sketch `.ino` si no es necesario (Arduino IDE suele esperar esa estructura).

---

## 12) Problemas comunes

### Error: `fatal: not a git repository`

Si aparece:

```txt
fatal: not a git repository (or any of the parent directories): .git
```

Significa que **no estás dentro de la carpeta del proyecto**.

Solución:

```txt
cd /c/Users/fcoib/Desktop/Oximetro
```

Luego vuelve a ejecutar:

```txt
git status
```

---

### Error al cargar el código al Arduino

Si Arduino IDE no permite cargar el código, revisa:

- Que el Arduino esté conectado por USB.
- Que la **placa** seleccionada sea correcta.
- Que el **puerto COM** seleccionado sea el correcto.
- Que ningún otro programa esté usando el puerto serial.
- Que el código compile correctamente (paso 6).

---

## 13) Flujo recomendado de trabajo (resumen)

1. En Git Bash:

```txt
cd /c/Users/fcoib/Desktop/Oximetro
git pull
```

2. Abre Arduino IDE, modifica el código y guarda:
- **Ctrl + S**

3. Sube cambios:

```txt
git status
git add .
git commit -m "Descripcion del cambio"
git push
```

