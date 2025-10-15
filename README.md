** pdg2025-fcen-uba-TP-4 **
** 2025 Procesamiento de Geometría Digital (Digital Geometry Processing) **

Trabajo Practico 4

Los archivos están organizados como en el Trabajo Practic 1 y el
proceso de compilación es el mismo.

Antes de compilar la aplicación por primera vez, edite el archivo
src/gui/GuiStrings.hpp o cópielo de tareas anteriores.

Debe implementar varios métodos en la nueva clase core/Optimization y,
para completar el programa de prueba de línea de comandos,
tst/dgpTest4.

Se incluyen instrucciones adicionales como comentarios en los archivos
fuente core/Optimization.hpp y core/Optimization.cpp.

Los archivos de las clases core/Faces, core/HalfEdges y
core/PolygonMesh no estan implementados en este repositorio. Debe
copiar su versión de estos archivos del se trabajo practico 3 en el
directorio core.

Se han realizado varios cambios en otras clases y se han añadido
nuevas clases al repositorio. No fusione otros archivos antiguos, ya
que la aplicación podría no compilar.

Tenga en cuenta que he añadido cargadores y guardadores para las
extensiones de archivo .obj y .off. No se admiten las especificaciones
completas para estos formatos, pero este código puede usarse para
cargar archivos exportados desde OpenSCAD, por ejemplo. Puede crear
sus propios ejemplos con OpenSCAD, pero tenga en cuenta que las
triangulaciones resultantes pueden ser de baja calidad si los modelos
tienen parches de superficie plana. Puede agregar vectores normales en
nuestra aplicación después de la carga y luego guardar la malla con
las normales de las caras como un archivo wrl. Finalmente, puede usar
el archivo wrl guardado como entrada para ahorrar tiempo al repetir
los experimentos.

Esta es la lista de tareas de esta tarea. Puede parecer extensa, pero
los métodos están estrechamente relacionados y proporciono muchas
sugerencias. Le sugiero que realice los cambios mínimos necesarios en
Optimization.cpp para que la aplicación compile y ejecute su código
anterior, y luego implemente una función a la vez.

// TAREAS DE SUAVIZADO (SMOOTHING)
//
// 1) void Optimization::laplacianSmoothingVertexCoordinatesRun();
//
// 2) void Optimization::laplacianSmoothingFaceNormalsRun(const bool normalize);
//
// 3) void Optimization::jacobiRun();
//
// TAREAS DE REMALLADO
//
// 4) void Optimization::_collapseEdgesSelect
//    (const EdgeCollapseIndependentSet indepSet,
//    vector<int>& edgeSelection,
//    bool colorIncidentFaces);
//
//    Optimization::_collapseEdgesSelect está parcialmente implementado
//
// 5) void Optimization::collapseEdgesApply
//    (const EdgeCollapseIndependentSet indepSet);
//
// 6) void Optimization::_adaptiveSubdivisionSelect
//    (vector<int>& vertexSelection,
//    const SplitEdgesMode mode, bool colorIncidentFaces);
//
// 7) void Optimization::adaptiveSubdivisionApply
//    (const SplitEdgesMode mode, const bool colorFaces);
//
// Puedes obtener algunas ideas para las tareas anteriores a partir de
// estos ejemplos
//
// YA IMPLEMENTADO
//    void Optimization::_equalizeValencesSelect
//    (vector<int>& edgeSelection, bool colorIncidentFaces);
//
// YA IMPLEMENTADO
//    void Optimization::equalizeValencesApply();

Envío:

  Como siempre,
  a) Localiza y elimina los archivos temporales creados por tu editor.
  b) Elimina el contenido de los subdirectorios superiores bin y build.
  c) Elimina el archivo qt/DGP2023-A4.pro.user creado por QtCreator.
  d) Elimina el directorio principal de build creado por QtCreator.
  e) Confirma todos tus cambios a intervalos regulares y en los hitos.
  f) Sube tus confirmaciones a tu repositorio de GitHub.
  g) Sigue las instrucciones de envío de tareas en GitHub Classroom.
     y envía la tarea antes de la fecha límite.
