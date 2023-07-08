SET int MAIN line 300 to your 'image.png' path:
unsigned char *data = stbi_load("D:/LabCompiler_CG/glfw-master/OwnProjects/rubick_finalista/imagen.png", &width, &height, &nrChannels, 0);


Run and control with keys
Control de movimiento de cámara: Si se presionan las teclas de flecha hacia arriba (GLFW_KEY_UP), hacia abajo (GLFW_KEY_DOWN), 
hacia la izquierda (GLFW_KEY_LEFT) o hacia la derecha (GLFW_KEY_RIGHT), los valores de pitch y yaw se ajustan según la velocidad de la cámara (cameraSpeed). 
Estas variables probablemente se utilizan para controlar la orientación de la cámara y se actualizan para lograr el movimiento deseado.
