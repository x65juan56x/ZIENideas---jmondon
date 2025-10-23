Juan Mondón - jmondon

## CONTEXTO:
Solo sé programar en C y un poco en C++, voy a elegir Python porque creo que está mas relacionado con estos lenguajes.

Empezaré con pruebas muy básicas y a medida que vaya funcionando iré aplicando las comprobaciones pertinentes.

### entrada
En principio la interface va a ser la consola, lo plantearé en C y luego con ayuda de coilot lo pasaré a python.
Una vez tenga el prompt con la idea para el cuento necesito conectarme con una IA que evalue que el prompt tiene sentido antes de procesarlo e intentar crear el cuento
He instalado la librería libcurl para poder conectar C con una IA
Intento usar la misma IA con la que voy a generar el cuento el prompt de entrada
Me estoy deteniendo mucho en el tema de comunicarme con la IA ya que nunca lo hice desde C
Ya tengo planteada esta parte

### generación
Tengo que averiguar como comunicar mi programa con una IA
Generación de un primer texto

### validación
Validar estructura (longitud, parrafos, intro nudo desenlace)

### iteración
Ajustar y repetir
Validar estructura (longitud, parrafos, intro nudo desenlace)
Ajustar y repetir
Validar estructura (longitud, parrafos, intro nudo desenlace)

### exportación
Formatear de manera coherente para presentar en un pdf

### portada


---

## FLOWCHART

- Leer prompt válido desde STDIN
- Validación del prompt:
	- Construir mensaje de validación (pide “OK: …” o “KO: …”).
	- Enviar a la IA y recoger la respuesta.
	- Extraer generated_text y decidir OK/KO.
- Generación del cuento:
	- Construir mensaje con requisitos:
		- ~500 palabras (±10%).
		- 3 partes: Presentación, Nudo, Desenlace.
		- Al menos 3 personajes distintos.
	- Enviar a la IA y mostrar solo el cuento.
- Validación:
	- Hacer una lectura crítica analítica del texto recibido para proponer reescritura o ajustes
	(en un principio se haría con la misma IA, pero con mas tiempo estería bueno que este análisis lo haga otra IA, poder usar distintos modelos para la validación).
- Iterar:
	- En función de los resultados de la validación, volver a mandar el texto para hacer ajustes, volver a validar.
	- Hacer este proceso dos veces más en función de lograr mejores resultados.
- Formateo:
	- Una vez conseguido el texto definitivo, se le pide a una IA que lo formatee de manera adecuada para presentarlo en un PDF

