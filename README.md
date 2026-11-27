# Tercer Departamental - Abstracción y Uso de Datos

## Carátula

| Dato | Información |
| --- | --- |
| Institución | Unidad Profesional Interdisciplinaria de Ingeniería y Ciencias Sociales y Administrativas |
| Asignatura | Abstracción y Uso de Datos |
| Periodo | Tercer Departamental |
| Equipo | Los Sin Oficio |
| Integrantes | Cruz Montalvo Erick Itzmin |
|  | Murillo Sánchez Fátima Mariana |
|  | Pérez Rivera Leia Michelle |
| Docente | Yventz Entzana Garduño |
| Repositorio | https://github.com/LoSinOficio/3er_Departamental.git |

---

## Índice

1. [Desarrollo](#1-desarrollo)
2. [Prácticas ](#2-practicas)
3. [Compilación y ejecución](#3-compilación-y-ejecución)
4. [Conclusiones](#4-conclusiones)
5. [Referencias](#5-referencias)

---

## 1. Desarrollo

### Descripción del trabajo

Este repositorio contiene las prácticas desarrolladas para el tercer departamental. Los ejercicios aplican conceptos de abstracción, programación orientada a objetos, persistencia en archivos y estructuras de datos como grafos, digrafos y árboles.

Cada práctica está organizada en un archivo independiente de C++ y dispone de su propia función `main`, por lo que puede compilarse y ejecutarse por separado.

### Objetivo

El objetivo principal del proyecto es reforzar el diseño de software y la manipulación de datos mediante la resolución de problemas que involucren:

- Tipos de dato abstracto (ADT).
- Persistencia de información en archivos TXT, XML, JSON y CSV.
- Implementación de grafos y digrafos con vértices, aristas, arcos y rutas.
- Construcción y manipulación de árboles jerárquicos.
- Aplicación de principios de diseño como SRP, OCP, LSP y DIP.

### Procedimiento

1. Se identificó el tipo de ejercicio y la estructura de datos requerida.
2. Se definieron las clases, atributos y operaciones del tipo de dato abstracto.
3. Se diseñó la interfaz de persistencia para guardar y recuperar información.
4. Se implementaron los menús interactivos con operaciones de creación, consulta, edición y eliminación.
5. Se compiló y ejecutó cada práctica de forma independiente.
6. Se verificó la persistencia de los datos y el comportamiento de las estructuras.

---

## 2. Prácticas implementadas

| Práctica | Tema | Archivo | Persistencia |
| --- | --- | --- | --- |
| 1 | Sistema escolar mediante ADT | [Practica_1.cpp](Practica_1.cpp) | CSV |
| 2 | ADT y persistencia con archivos TXT | [Practica_2.cpp](Practica_2.cpp) | TXT |
| 3 | ADT y persistencia con archivos JSON | [Practica_3.cpp](Practica_3.cpp) | JSON |
| 5 | Grafos y paths | [practica5_gr.cpp](practica5_gr.cpp) | JSON |
| 6 | Digrafos y paths | [Practica6_Di.cpp](Practica6_Di.cpp) | JSON |
| 7 | Árboles jerárquicos | [Practica7_Ar.cpp](Practica7_Ar.cpp) | JSON |
| 11 | ADT y persistencia con archivos CSV | [Practica_11.cpp](Practica_11.cpp) | CSV |

### 2.1 Persistencia de datos

Las prácticas trabajan con elementos encapsulados en clases ADT y permiten administrar información en archivos XML, TXT, JSON y CSV. Las prácticas 1 y 11 contienen sistemas escolares basados en registros; permiten agregar, listar, editar y eliminar información en memoria y guardarla o cargarla desde disco. Las demás prácticas usan formatos específicos para conservar sus estructuras y datos.

### 2.2 Grafos

La práctica 5 administra múltiples grafos. Cada grafo contiene:

- Un conjunto de vértices `V`.
- Tramos o aristas `A`, con origen, destino, etiqueta, tiempo y costo.
- Un `path` construido como una lista de nodos.
- Persistencia de todos los grafos en `grafos_cuaderno.json`.

Permite crear y seleccionar grafos, administrar vértices y rutas, guardar y cargar datos. Además, el menú incluye las opciones «Editar elemento existente en memoria RAM» y «Eliminar elemento existente en memoria RAM», con las que se pueden modificar o retirar vértices y tramos del grafo activo.

### 2.3 Digrafos

La práctica 6 administra múltiples digrafos. Cada estructura contiene vértices, arcos dirigidos y un path. Los arcos incluyen nodo inicial, nodo final, etiqueta, tiempo y costo.

Los datos se almacenan en el archivo `digrafos_cuaderno.json`, y el menú permite crear, editar, eliminar y consultar cada digrafo.

### 2.4 Árboles

La práctica 7 administra múltiples árboles mediante una estructura compuesta por nodos padre e hijos. Permite establecer una raíz, agregar hijos, modificar nodos, eliminar un nodo con su subárbol y visualizar la jerarquía.

Los datos se guardan y recuperan desde `arboles_cuaderno.json`.

### Ejemplo de código esencial

Las prácticas utilizan clases que encapsulan los datos y separan la lógica del sistema de la forma en que se realiza la persistencia:

```cpp
class ElementoADT {
private:
    int id;
    string nombre;
    vector<int> calificaciones;

public:
    ElementoADT(int id, string nombre, vector<int> cals)
        : id(id), nombre(nombre), calificaciones(cals) {}

    int getId() const { return id; }
    string getNombre() const { return nombre; }
    const vector<int>& getCalificaciones() const { return calificaciones; }
};
```

Las operaciones de almacenamiento se abstraen mediante interfaces y manejadores específicos. De esta forma, el programa puede guardar y recuperar registros sin acoplar directamente la lógica principal a un formato de archivo concreto.

### Operaciones disponibles en memoria

Los menúes interactivos incluyen operaciones CRUD según la estructura:

- Agregar nuevos registros, vértices, tramos, arcos o nodos.
- Consultar y mostrar los elementos registrados.
- Editar elementos mediante «Editar elemento existente en memoria RAM».
- Eliminar elementos mediante «Eliminar elemento existente en memoria RAM».
- Guardar el estado actual y cargarlo posteriormente desde el archivo correspondiente.

---

## 3. Compilación y ejecución

Se requiere un compilador compatible con C++11 o posterior, como `g++`.

### Compilar una práctica

Ejecuta desde la raíz del repositorio:

```powershell
g++ Practica_1.cpp -o practica1
g++ Practica_2.cpp -o practica2
g++ Practica_3.cpp -o practica3
g++ practica5_gr.cpp -o practica5
g++ Practica6_Di.cpp -o practica6
g++ Practica7_Ar.cpp -o practica7
g++ Practica_11.cpp -o practica11
```

### Ejecutar en Windows

```powershell
.\practica1.exe
.\practica2.exe
.\practica3.exe
.\practica5.exe
.\practica6.exe
.\practica7.exe
.\practica11.exe
```

Cada archivo contiene su propia función `main`, por lo que debe compilarse de forma independiente.

---

## 4. Conclusiones

El tercer departamental permitió reforzar conceptos fundamentales de abstracción, programación orientada a objetos y persistencia de datos. Mediante los ejercicios se comprendió la importancia de separar la estructura de la información de su almacenamiento y de organizar operaciones mediante clases y interfaces.

Las prácticas de grafos, digrafos y árboles permitieron aplicar estructuras no lineales y administrar colecciones mediante menús interactivos y archivos JSON. El uso combinado de ADT y persistencia produce soluciones más modulares, reutilizables y fáciles de mantener.

---

## 5. Referencias

- [Practica_1.cpp](Practica_1.cpp): sistema escolar con persistencia CSV.
- [Practica_2.cpp](Practica_2.cpp): persistencia TXT.
- [Practica_3.cpp](Practica_3.cpp): persistencia JSON.
- [practica5_gr.cpp](practica5_gr.cpp): grafos y paths.
- [Practica6_Di.cpp](Practica6_Di.cpp): digrafos y paths.
- [Practica7_Ar.cpp](Practica7_Ar.cpp): árboles jerárquicos.
- [Practica_11.cpp](Practica_11.cpp): persistencia CSV.
- Bibliotecas estándar de C++: `<iostream>`, `<fstream>`, `<sstream>`, `<vector>`, `<list>`, `<map>`, `<memory>` y `<string>`.
- Material académico de la asignatura Abstracción y Uso de Datos.

---

Repositorio académico desarrollado para la asignatura de Abstracción y Uso de Datos.
