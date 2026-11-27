#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

// ==========================================
// 1. DATO / ELEMENTO COMPLEJO (SRP)
// Reutilizable para dato complejo y arreglo complejo
// ==========================================
class ItemComplejo {
public:
    string clave;
    int nivel;
    double peso;

    ItemComplejo(string c = "GEN", int n = 1, double p = 1.0)
        : clave(c), nivel(n), peso(p) {}

    string aXML(const string& tag, int indent = 8) const {
        string pad(indent, ' ');
        string inner(indent + 4, ' ');
        return pad + "<" + tag + ">\n" +
               inner + "<clave>" + clave + "</clave>\n" +
               inner + "<nivel>" + to_string(nivel) + "</nivel>\n" +
               inner + "<peso>" + to_string(peso) + "</peso>\n" +
               pad + "</" + tag + ">\n";
    }

    void mostrar() const {
        cout << "[" << clave << " N:" << nivel << " P:" << peso << "]";
    }
};

// ==========================================
// 2. TIPO DE DATO ABSTRACTO (ADT) - SRP
// Basico + arreglo simple + dato complejo + arreglo complejo
// ==========================================
class ElementoADT {
private:
    int id;
    string nombre;
    vector<int> simples;
    ItemComplejo complejo;
    vector<ItemComplejo> arreglosComplejos;

public:
    ElementoADT(int id,
                string nombre,
                vector<int> s,
                ItemComplejo c,
                vector<ItemComplejo> ac)
        : id(id), nombre(nombre), simples(s), complejo(c), arreglosComplejos(ac) {}

    int getId() const { return id; }
    string getNombre() const { return nombre; }
    const vector<int>& getSimples() const { return simples; }
    const ItemComplejo& getComplejo() const { return complejo; }
    const vector<ItemComplejo>& getArreglosComplejos() const { return arreglosComplejos; }

    string aXML() const {
        string xml = "    <elemento>\n";
        xml += "        <id>" + to_string(id) + "</id>\n";
        xml += "        <nombre>" + nombre + "</nombre>\n";

        xml += "        <simples>\n";
        for (int s : simples) {
            xml += "            <valor>" + to_string(s) + "</valor>\n";
        }
        xml += "        </simples>\n";

        xml += "        <dato_complejo>\n";
        xml += complejo.aXML("item", 12);
        xml += "        </dato_complejo>\n";

        xml += "        <arreglos_complejos>\n";
        for (const auto& ac : arreglosComplejos) {
            xml += ac.aXML("item", 12);
        }
        xml += "        </arreglos_complejos>\n";

        xml += "    </elemento>";
        return xml;
    }
};

// ==========================================
// 3. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaXML {
public:
    virtual void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) = 0;
    virtual vector<shared_ptr<ElementoADT>> leer(const string& ruta) = 0;
    virtual ~IPersistenciaXML() = default;
};

// ==========================================
// 4. IMPLEMENTACION CONCRETA - LSP / SRP
// ==========================================
class ManejadorXML : public IPersistenciaXML {
private:
    static string extraerTag(const string& linea, const string& tag) {
        string abre = "<" + tag + ">";
        string cierra = "</" + tag + ">";
        size_t ini = linea.find(abre);
        size_t fin = linea.find(cierra);
        if (ini == string::npos || fin == string::npos) return "";
        ini += abre.size();
        return linea.substr(ini, fin - ini);
    }

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
        string nombre;
        vector<int> simples;
        ItemComplejo complejo;
        vector<ItemComplejo> arreglosComplejos;
        ItemComplejo itemActual;

        bool enElemento = false;
        bool enSimples = false;
        bool enDatoComplejo = false;
        bool enArreglosComplejos = false;
        bool enItem = false;

        while (getline(archivo, linea)) {
            if (linea.find("<elemento>") != string::npos) {
                id = 0;
                nombre.clear();
                simples.clear();
                complejo = ItemComplejo();
                arreglosComplejos.clear();
                enElemento = true;
            }
            else if (linea.find("</elemento>") != string::npos) {
                elementos.push_back(make_shared<ElementoADT>(id, nombre, simples, complejo, arreglosComplejos));
                enElemento = false;
            }
            else if (enElemento) {
                if (linea.find("<simples>") != string::npos) enSimples = true;
                else if (linea.find("</simples>") != string::npos) enSimples = false;
                else if (linea.find("<dato_complejo>") != string::npos) enDatoComplejo = true;
                else if (linea.find("</dato_complejo>") != string::npos) enDatoComplejo = false;
                else if (linea.find("<arreglos_complejos>") != string::npos) enArreglosComplejos = true;
                else if (linea.find("</arreglos_complejos>") != string::npos) enArreglosComplejos = false;
                else if (linea.find("<item>") != string::npos) {
                    itemActual = ItemComplejo();
                    enItem = true;
                }
                else if (linea.find("</item>") != string::npos) {
                    if (enDatoComplejo) complejo = itemActual;
                    else if (enArreglosComplejos) arreglosComplejos.push_back(itemActual);
                    enItem = false;
                }
                else if (enItem) {
                    string c = extraerTag(linea, "clave");
                    string n = extraerTag(linea, "nivel");
                    string p = extraerTag(linea, "peso");
                    if (!c.empty()) itemActual.clave = c;
                    if (!n.empty()) itemActual.nivel = stoi(n);
                    if (!p.empty()) itemActual.peso = stod(p);
                }
                else if (enSimples) {
                    string v = extraerTag(linea, "valor");
                    if (!v.empty()) simples.push_back(stoi(v));
                }
                else {
                    string idStr = extraerTag(linea, "id");
                    string nomStr = extraerTag(linea, "nombre");
                    if (!idStr.empty()) id = stoi(idStr);
                    if (!nomStr.empty()) nombre = nomStr;
                }
            }
        }

        archivo.close();
        cout << "\n[EXITO] Datos leidos y recuperados desde el archivo XML: " << ruta << "\n";
        return elementos;
    }
};

// ==========================================
// 5. CLASE GESTORA DEL SISTEMA (POO)
// ==========================================
class GestorSistema {
private:
    vector<shared_ptr<ElementoADT>> coleccionDatos;
    unique_ptr<IPersistenciaXML> persistencia;

public:
    GestorSistema(unique_ptr<IPersistenciaXML> pers) : persistencia(move(pers)) {}

    void agregarElemento(int id,
                         string nombre,
                         vector<int> simples,
                         ItemComplejo complejo,
                         vector<ItemComplejo> arreglosComplejos) {
        coleccionDatos.push_back(
            make_shared<ElementoADT>(id, nombre, simples, complejo, arreglosComplejos)
        );
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
        cout << "\n=======================================================================\n";
        cout << "                    REGISTROS ACTUALES EN MEMORIA                      \n";
        cout << "=======================================================================\n";
        for (const auto& elem : coleccionDatos) {
            cout << "ID: " << elem->getId()
                 << " | Nombre: " << elem->getNombre()
                 << " | Simples: [ ";
            for (int s : elem->getSimples()) {
                cout << s << " ";
            }
            cout << "]\n  Dato Complejo: ";
            elem->getComplejo().mostrar();
            cout << "\n  Arreglo Complejo: [ ";
            for (size_t i = 0; i < elem->getArreglosComplejos().size(); ++i) {
                elem->getArreglosComplejos()[i].mostrar();
                if (i + 1 < elem->getArreglosComplejos().size()) cout << ", ";
            }
            cout << " ]\n";
        }
        cout << "=======================================================================\n";
    }
};

// ==========================================
// 6. FUNCION PRINCIPAL CON MENU INTERACTIVO
// ==========================================
int main() {
    unique_ptr<IPersistenciaXML> servicioXML = make_unique<ManejadorXML>();
    GestorSistema sistema(move(servicioXML));

    int opcion;
    string nombreArchivo = "ejercicio_datos_completo.xml";

    do {
        cout << "\n=========================================================\n";
        cout << "   MENU - ARCHIVOS XML  \n";
        cout << "=========================================================\n";
        cout << "1. Agregar nuevo elemento (ADT)\n";
        cout << "2. Ver elementos en memoria RAM\n";
        cout << "3. Guardar elementos en archivo XML (Disco)\n";
        cout << "4. Cargar elementos desde archivo XML (Disco)\n";
        cout << "5. Salir\n";
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                int id, cantSimples, cantComplejos;
                string nombre, cClave;
                int cNivel;
                double cPeso;

                cout << "Ingresa el ID numerico: ";
                cin >> id;
                cout << "Ingresa el Nombre o Descriptor: ";
                cin >> nombre;

                cout << "Cuantos valores tendra el arreglo simple?: ";
                cin >> cantSimples;
                vector<int> simples(cantSimples);
                for (int i = 0; i < cantSimples; ++i) {
                    cout << "  Valor simple [" << i + 1 << "]: ";
                    cin >> simples[i];
                }

                cout << "Dato complejo (clave nivel peso): ";
                cin >> cClave >> cNivel >> cPeso;
                ItemComplejo complejo(cClave, cNivel, cPeso);

                cout << "Cuantos objetos tendra el arreglo complejo?: ";
                cin >> cantComplejos;
                vector<ItemComplejo> arreglosComplejos;
                for (int i = 0; i < cantComplejos; ++i) {
                    string k;
                    int nv;
                    double ps;
                    cout << "  Item complejo [" << i + 1 << "] (clave nivel peso): ";
                    cin >> k >> nv >> ps;
                    arreglosComplejos.emplace_back(k, nv, ps);
                }

                sistema.agregarElemento(id, nombre, simples, complejo, arreglosComplejos);
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
                cout << "\nSaliendo del programa. Hasta luego!\n";
                break;
            default:
                cout << "\n[ERROR] Opcion no valida. Intenta de nuevo.\n";
        }
    } while (opcion != 5);

    return 0;
}