# jonOS
Sencillo sistema operativo para crear sistemas dedicados.

## Decisiones de implementación de jonOS

#### Acceso a memoria
En un principio había pensado en implementar la MMU para poder utilizar direcciones virtuales, pero eso provocaría una bajada de rendimiento respecto al acceder a las direcciones físicas directamente. Es por eso por lo que he decidido que el SO trabaje con direcciones físicas en lugar de virtuales.

#### Estructura del kernel
El kernel comienza en la dirección 0x8000 y es en esa dirección donde encontramos las secciones .text, .rodata, .data, .bss por ese orden y alineadas cada una de ellas a 4096 Bytes. Después de la sección .bss he decidido crear una sección para el heap del kernel de 4MB de tamaño. Y justo después .heap irá el código de la función que el cliente quiere que ejecute.

#### Acceso a la red
He encontrado una librería que se llama lwip (Light-weight IP) y que es una implementación reducida del protocolo TCP/IP además de muchos otros.
