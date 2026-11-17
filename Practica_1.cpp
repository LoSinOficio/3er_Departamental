#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

// ==========================================
// 1. TIPO DE DATO ABSTRACTO (ADT) - SRP
// ==========================================
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

    // Método para serializar el objeto a formato XML
    string aXML() const {
        string xml = "    <elemento>\n";
        xml += "        <id>" + to_string(id) + "</id>\n";
        xml += "        <nombre>" + nombre + "</nombre>\n";
        xml += "        <calificaciones>\n";
        for (int cal : calificaciones) {
            xml += "            <valor>" + to_string(cal) + "</valor>\n";
        }
        xml += "        </calificaciones>\n";
        xml += "    </elemento>";
        return xml;
    }
};

// ==========================================
// 2. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaXML {
public:
    virtual void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) = 0;
    virtual vector<shared_ptr<ElementoADT>> leer(const string& ruta) = 0;
    virtual ~IPersistenciaXML() = default;
};

// ==========================================
// 3. IMPLEMENTACIÓN CONCRETA - LSP / SRP
// ==========================================
class ManejadorXML : public IPersistenciaXML {
public:
    void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo XML para escritura.\n";
            return;
        }

        archivo << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        archivo << "<sistema>\n";
        for (const auto& elem : elementos) {
            archivo << elem->aXML() << "\n";
        }
        archivo << "</sistema>\n";

        archivo.close();
        cout << "\n[EXITO] Datos guardados correctamente en el archivo XML: " << ruta << "\n";
    }

    vector<shared_ptr<ElementoADT>> leer(const string& ruta) override {
        vector<shared_ptr<ElementoADT>> elementos;
        ifstream archivo(ruta);
        
        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro un archivo XML previo en la ruta. Se iniciara vacio.\n";
            return elementos;
        }

        string linea;
        int id = 0;
        string nombre = "";
        vector<int> cals;
        bool leyendoElemento = false;
        bool leyendoCals = false;

        while (getline(archivo, linea)) {
            if (linea.find("<elemento>") != string::npos) {
                id = 0;
                nombre = "";
                cals.clear();
                leyendoElemento = true;
            }
            else if (linea.find("</elemento>") != string::npos) {
                elementos.push_back(make_shared<ElementoADT>(id, nombre, cals));
                leyendoElemento = false;
            }
            else if (leyendoElemento) {
                if (linea.find("<id>") != string::npos) {
                    size_t inicio = linea.find("<id>") + 4;
                    size_t fin = linea.find("</id>");
                    id = stoi(linea.substr(inicio, fin - inicio));
                }
                else if (linea.find("<nombre>") != string::npos) {
                    size_t inicio = linea.find("<nombre>") + 8;
                    size_t fin = linea.find("</nombre>");
                    nombre = linea.substr(inicio, fin - inicio);
                }
                else if (linea.find("<calificaciones>") != string::npos) {
                    leyendoCals = true;
                }
                else if (linea.find("</calificaciones>") != string::npos) {
                    leyendoCals = false;
                }
                else if (leyendoCals && linea.find("<valor>") != string::npos) {
                    size_t inicio = linea.find("<valor>") + 7;
                    size_t fin = linea.find("</valor>");
                    cals.push_back(stoi(linea.substr(inicio, fin - inicio)));
                }
            }
        }

        archivo.close();
        cout << "\n[EXITO] Datos leidos y recuperados desde el archivo XML: " << ruta << "\n";
        return elementos;
    }
};

// ==========================================
// 4. CLASE GESTORA DEL SISTEMA (POO)
// ==========================================
class GestorSistema {
private:
    vector<shared_ptr<ElementoADT>> coleccionDatos;
    unique_ptr<IPersistenciaXML> persistencia;

public:
    GestorSistema(unique_ptr<IPersistenciaXML> pers) : persistencia(move(pers)) {}

    void agregarElemento(int id, string nombre, vector<int> cals) {
        coleccionDatos.push_back(make_shared<ElementoADT>(id, nombre, cals));
        cout << "\n[INFO] Elemento agregado exitosamente a la memoria RAM.\n";
    }

    void guardarEnDisco(const string& ruta) {
        persistencia->escribir(ruta, coleccionDatos);
    }

    void cargarDesdeDisco(const string& ruta) {
        coleccionDatos = persistencia->leer(ruta);
    }

    void mostrarEnConsola() const {
        if (coleccionDatos.empty()) {
            cout << "\n[AVISO] No hay elementos registrados en la memoria actualmente.\n";
            return;
        }
        cout << "\n=========================================\n";
        cout << "       REGISTROS ACTUALES EN MEMORIA     \n";
        cout << "=========================================\n";
        for (const auto& elem : coleccionDatos) {
            cout << "ID: " << elem->getId() 
                 << " | Nombre: " << elem->getNombre() 
                 << " | Arreglo de Valores: [ ";
            for (int c : elem->getCalificaciones()) {
                cout << c << " ";
            }
            cout << "]\n";
        }
        cout << "=========================================\n";
    }
};

// ==========================================
// 5. FUNCION PRINCIPAL CON MENU INTERACTIVO
// ==========================================
int main() {
    unique_ptr<IPersistenciaXML> servicioXML = make_unique<ManejadorXML>();
    GestorSistema sistema(move(servicioXML));

    int opcion;
    string nombreArchivo = "ejercicio_datos.xml";

    do {
        cout << "\n=========================================\n";
        cout << "     MENU  - ARCHIVOS XML     \n";
        cout << "=========================================\n";
        cout << "1. Agregar nuevo elemento (ADT)\n";
        cout << "2. Ver elementos en memoria RAM\n";
        cout << "3. Guardar elementos en archivo XML (Disco)\n";
        cout << "4. Cargar elementos desde archivo XML (Disco)\n";
        cout << "5. Salir\n";
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                int id;
                string nombre;
                int v1, v2, v3;

                cout << "Ingresa el ID numerico: ";
                cin >> id;
                cout << "Ingresa el Nombre o Descriptor: ";
                cin >> nombre;
                cout << "Ingresa 3 valores numericos para el arreglo (separados por espacio): ";
                cin >> v1 >> v2 >> v3;

                vector<int> arreglo = {v1, v2, v3};
                sistema.agregarElemento(id, nombre, arreglo);
                break;
            }
            case 2:
                sistema.mostrarEnConsola();
                break;
            case 3:
                sistema.guardarEnDisco(nombreArchivo);
                break;
            case 4:
                sistema.cargarDesdeDisco(nombreArchivo);
                break;
            case 5:
                cout << "\nSaliendo del programa. ¡Hasta luego!\n";
                break;
            default:
                cout << "\n[ERROR] Opcion no valida. Intenta de nuevo.\n";
        }
    } while (opcion != 5);

    return 0;
}