#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <limits>
#include <algorithm>

using namespace std;

// ==========================================
// 1. REGISTRO ESCOLAR DESCRIPTIVO (SRP)
// Permite describir alumnos, asignaturas, aulas, equipos, etc.
// con multiples descripciones por registro
// ==========================================
class RegistroEscolar {
private:
    string tipo;
    string etiqueta;
    vector<string> descripciones;

    static string limpiar(const string& s) {
        size_t ini = s.find_first_not_of(" \t\r\n");
        if (ini == string::npos) return "";
        size_t fin = s.find_last_not_of(" \t\r\n");
        return s.substr(ini, fin - ini + 1);
    }

    static string escaparCampo(const string& s) {
        string out;
        for (char c : s) {
            if (c == '\\') out += "\\\\";
            else if (c == '|') out += "\\p";
            else if (c == ';') out += "\\s";
            else if (c == ',') out += "\\c";
            else if (c == '~') out += "\\t";
            else if (c == '\n' || c == '\r') out += " ";
            else out += c;
        }
        return out;
    }

    static string desescaparCampo(const string& s) {
        string out;
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\\' && i + 1 < s.size()) {
                if (s[i + 1] == '\\') { out += '\\'; ++i; }
                else if (s[i + 1] == 'p') { out += '|'; ++i; }
                else if (s[i + 1] == 's') { out += ';'; ++i; }
                else if (s[i + 1] == 'c') { out += ','; ++i; }
                else if (s[i + 1] == 't') { out += '~'; ++i; }
                else out += s[i];
            } else {
                out += s[i];
            }
        }
        return out;
    }

public:
    RegistroEscolar(string t = "asignatura",
                    string etq = "sin_etiqueta",
                    vector<string> descs = {})
        : tipo(move(t)), etiqueta(move(etq)), descripciones(move(descs)) {}

    const string& getTipo() const { return tipo; }
    const string& getEtiqueta() const { return etiqueta; }
    const vector<string>& getDescripciones() const { return descripciones; }

    void agregarDescripcion(const string& desc) {
        string limp = limpiar(desc);
        if (!limp.empty()) {
            descripciones.push_back(limp);
        }
    }

    // Formato interno: tipo|etiqueta|desc1~desc2~desc3
    string serializar() const {
        string tipoFinal = limpiar(tipo).empty() ? "asignatura" : limpiar(tipo);
        string etqFinal = limpiar(etiqueta).empty() ? "sin_etiqueta" : limpiar(etiqueta);

        string out = escaparCampo(tipoFinal) + "|" + escaparCampo(etqFinal) + "|";
        for (size_t i = 0; i < descripciones.size(); ++i) {
            out += escaparCampo(descripciones[i]);
            if (i + 1 < descripciones.size()) out += "~";
        }
        return out;
    }

    static RegistroEscolar deserializar(const string& texto) {
        stringstream ss(texto);
        string tStr, etqStr, descsStr;
        getline(ss, tStr, '|');
        getline(ss, etqStr, '|');
        getline(ss, descsStr, '|');

        string tLimp = limpiar(desescaparCampo(tStr));
        string eLimp = limpiar(desescaparCampo(etqStr));

        vector<string> descs;
        stringstream ssDesc(descsStr);
        string itemDesc;
        while (getline(ssDesc, itemDesc, '~')) {
            string dLimp = limpiar(desescaparCampo(itemDesc));
            if (!dLimp.empty()) {
                descs.push_back(dLimp);
            }
        }

        return RegistroEscolar(
            tLimp.empty() ? "asignatura" : tLimp,
            eLimp.empty() ? "sin_etiqueta" : eLimp,
            move(descs)
        );
    }

    void mostrar() const {
        cout << "[" << tipo << ": " << etiqueta;
        if (!descripciones.empty()) {
            cout << " -> ";
            for (size_t i = 0; i < descripciones.size(); ++i) {
                cout << "\"" << descripciones[i] << "\"";
                if (i + 1 < descripciones.size()) cout << " / ";
            }
        }
        cout << "]";
    }
};

// ==========================================
// 2. TIPO DE DATO ABSTRACTO ESCOLAR (ADT) - SRP
// ID + Nombre/Grupo + 3 calificaciones/valores
// + arreglo de registros escolares + puntero al arreglo
// ==========================================
class ElementoEscolarADT {
private:
    int id;
    string nombre;
    vector<int> calificaciones;
    vector<RegistroEscolar> registros;
    const vector<RegistroEscolar>* ptrRegistros;

    void sincronizarPuntero() {
        ptrRegistros = &registros;
    }

public:
    ElementoEscolarADT(int id,
                       string nombre,
                       vector<int> cals,
                       vector<RegistroEscolar> regs = {})
        : id(id),
          nombre(move(nombre)),
          calificaciones(move(cals)),
          registros(move(regs)),
          ptrRegistros(nullptr) {
        sincronizarPuntero();
    }

    ElementoEscolarADT(const ElementoEscolarADT& otro)
        : id(otro.id),
          nombre(otro.nombre),
          calificaciones(otro.calificaciones),
          registros(otro.registros),
          ptrRegistros(nullptr) {
        sincronizarPuntero();
    }

    ElementoEscolarADT& operator=(const ElementoEscolarADT& otro) {
        if (this != &otro) {
            id = otro.id;
            nombre = otro.nombre;
            calificaciones = otro.calificaciones;
            registros = otro.registros;
            sincronizarPuntero();
        }
        return *this;
    }

    int getId() const { return id; }
    const string& getNombre() const { return nombre; }
    const vector<int>& getCalificaciones() const { return calificaciones; }
    const vector<RegistroEscolar>& getRegistros() const { return registros; }

    const vector<RegistroEscolar>* getPunteroRegistros() const {
        return ptrRegistros;
    }

    void mostrarRegistrosIndirecto() const {
        cout << "   -> Detalles escolares (via puntero): [ ";
        if (ptrRegistros == nullptr || ptrRegistros->empty()) {
            cout << "sin registros";
        } else {
            for (size_t i = 0; i < ptrRegistros->size(); ++i) {
                (*ptrRegistros)[i].mostrar();
                if (i + 1 < ptrRegistros->size()) cout << ", ";
            }
        }
        cout << " ]\n";
    }

    string aCSV() const {
        string linea = to_string(id) + "," + nombre + ",";
        for (size_t i = 0; i < calificaciones.size(); ++i) {
            linea += to_string(calificaciones[i]);
            if (i + 1 < calificaciones.size()) linea += ";";
        }

        linea += ",";
        if (ptrRegistros != nullptr) {
            for (size_t i = 0; i < ptrRegistros->size(); ++i) {
                linea += (*ptrRegistros)[i].serializar();
                if (i + 1 < ptrRegistros->size()) linea += ";";
            }
        }
        return linea;
    }
};

// ==========================================
// 3. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaEscuelaCSV {
public:
    virtual void escribir(const string& ruta, const vector<shared_ptr<ElementoEscolarADT>>& elementos) = 0;
    virtual vector<shared_ptr<ElementoEscolarADT>> leer(const string& ruta) = 0;
    virtual ~IPersistenciaEscuelaCSV() = default;
};

// ==========================================
// 4. IMPLEMENTACION CONCRETA - LSP / SRP
// ==========================================
class ManejadorEscuelaCSV : public IPersistenciaEscuelaCSV {
private:
    static string limpiarRetornoCarro(string s) {
        s.erase(remove(s.begin(), s.end(), '\r'), s.end());
        return s;
    }

    static bool parsearEnteroSeguro(const string& texto, int& salida) {
        if (texto.empty()) return false;
        try {
            size_t idx = 0;
            int val = stoi(texto, &idx);
            if (idx != texto.size()) return false;
            salida = val;
            return true;
        } catch (...) {
            return false;
        }
    }

public:
    void escribir(const string& ruta, const vector<shared_ptr<ElementoEscolarADT>>& elementos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo para escritura en: " << ruta << "\n";
            return;
        }

        archivo << "ID,Nombre,Calificaciones,RegistrosEscolares\n";
        for (const auto& elem : elementos) {
            archivo << elem->aCSV() << "\n";
        }

        archivo.close();
        cout << "\n[EXITO] Datos escolares guardados correctamente en: " << ruta << "\n";
    }

    vector<shared_ptr<ElementoEscolarADT>> leer(const string& ruta) override {
        vector<shared_ptr<ElementoEscolarADT>> elementos;
        ifstream archivo(ruta);

        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro un archivo previo en la ruta. Se iniciara vacio.\n";
            return elementos;
        }

        string linea;
        getline(archivo, linea); // Omitir cabecera

        while (getline(archivo, linea)) {
            linea = limpiarRetornoCarro(linea);
            if (linea.empty()) continue;

            stringstream ss(linea);
            string idStr, nombre, bloqueCals, bloqueRegs;

            if (!getline(ss, idStr, ',') || idStr.empty()) continue;
            int id = 0;
            if (!parsearEnteroSeguro(idStr, id)) continue;

            if (!getline(ss, nombre, ',')) continue;
            getline(ss, bloqueCals, ',');
            getline(ss, bloqueRegs, ',');

            vector<int> cals;
            stringstream ssCals(bloqueCals);
            string dato;
            while (getline(ssCals, dato, ';')) {
                int val = 0;
                if (parsearEnteroSeguro(dato, val)) {
                    cals.push_back(val);
                }
            }

            vector<RegistroEscolar> regs;
            stringstream ssRegs(bloqueRegs);
            string itemReg;
            while (getline(ssRegs, itemReg, ';')) {
                if (!itemReg.empty()) {
                    regs.push_back(RegistroEscolar::deserializar(itemReg));
                }
            }

            elementos.push_back(
                make_shared<ElementoEscolarADT>(id, nombre, move(cals), move(regs))
            );
        }

        archivo.close();
        cout << "\n[EXITO] Datos escolares leidos y recuperados desde: " << ruta << "\n";
        return elementos;
    }
};

// ==========================================
// 5. CLASE GESTORA DEL SISTEMA ESCOLAR (POO)
// ==========================================
class GestorEscuela {
private:
    vector<shared_ptr<ElementoEscolarADT>> coleccionDatos;
    unique_ptr<IPersistenciaEscuelaCSV> persistencia;

public:
    GestorEscuela(unique_ptr<IPersistenciaEscuelaCSV> pers) : persistencia(move(pers)) {}

    bool estaVacia() const {
        return coleccionDatos.empty();
    }

    size_t cantidadElementos() const {
        return coleccionDatos.size();
    }

    void agregarElemento(int id,
                         string nombre,
                         vector<int> cals,
                         vector<RegistroEscolar> regs) {
        coleccionDatos.push_back(
            make_shared<ElementoEscolarADT>(id, move(nombre), move(cals), move(regs))
        );
        cout << "\n[INFO] Registro escolar agregado exitosamente a la memoria RAM.\n";
    }

    bool editarElemento(size_t indice,
                        int id,
                        string nombre,
                        vector<int> cals,
                        vector<RegistroEscolar> regs) {
        if (indice >= coleccionDatos.size()) return false;
        coleccionDatos[indice] = make_shared<ElementoEscolarADT>(
            id, move(nombre), move(cals), move(regs)
        );
        cout << "\n[INFO] Registro escolar [" << (indice + 1) << "] actualizado en la memoria RAM.\n";
        return true;
    }

    bool eliminarElemento(size_t indice) {
        if (indice >= coleccionDatos.size()) return false;
        coleccionDatos.erase(coleccionDatos.begin() + static_cast<long>(indice));
        cout << "\n[INFO] Registro escolar [" << (indice + 1) << "] eliminado de la memoria RAM.\n";
        return true;
    }

    void guardarEnDisco(const string& ruta) {
        persistencia->escribir(ruta, coleccionDatos);
    }

    void cargarDesdeDisco(const string& ruta) {
        coleccionDatos = persistencia->leer(ruta);
    }

    void listarResumido() const {
        if (coleccionDatos.empty()) {
            cout << "\n[AVISO] No hay registros escolares en la memoria actualmente.\n";
            return;
        }
        cout << "\n--- REGISTROS ESCOLARES EN MEMORIA RAM ---\n";
        for (size_t i = 0; i < coleccionDatos.size(); ++i) {
            cout << (i + 1) << ". ID: " << coleccionDatos[i]->getId()
                 << " | Nombre/Grupo: " << coleccionDatos[i]->getNombre() << "\n";
        }
    }

    void mostrarEnConsola() const {
        if (coleccionDatos.empty()) {
            cout << "\n[AVISO] No hay registros escolares en la memoria actualmente.\n";
            return;
        }
        cout << "\n=======================================================================\n";
        cout << "                 REGISTROS ESCOLARES ACTUALES EN MEMORIA               \n";
        cout << "=======================================================================\n";
        for (size_t i = 0; i < coleccionDatos.size(); ++i) {
            const auto& elem = coleccionDatos[i];
            cout << "[" << (i + 1) << "] ID: " << elem->getId()
                 << " | Nombre/Grupo: " << elem->getNombre()
                 << " | Calificaciones/Valores: [ ";
            for (int c : elem->getCalificaciones()) {
                cout << c << " ";
            }
            cout << "]\n";
            elem->mostrarRegistrosIndirecto();
        }
        cout << "=======================================================================\n";
    }
};

namespace {
    void limpiarEntrada() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    int leerEntero(const string& prompt) {
        int val;
        while (true) {
            cout << prompt;
            if (cin >> val) return val;
            limpiarEntrada();
            cout << "[ERROR] Ingresa un numero entero valido.\n";
        }
    }

    string leerPalabra(const string& prompt) {
        string tok;
        cout << prompt;
        cin >> tok;
        return tok;
    }

    string leerLinea(const string& prompt) {
        string linea;
        cout << prompt;
        getline(cin >> ws, linea);
        return linea;
    }

    bool capturarDatosEscolares(int& id,
                                string& nombre,
                                vector<int>& arreglo,
                                vector<RegistroEscolar>& registros) {
        int v1, v2, v3;
        id = leerEntero("Ingresa el ID (matricula o folio): ");
        nombre = leerPalabra("Ingresa el Nombre o Grupo (sin espacios): ");

        cout << "Ingresa 3 calificaciones o valores numericos (separados por espacio): ";
        cin >> v1 >> v2 >> v3;
        if (cin.fail()) {
            limpiarEntrada();
            cout << "\n[ERROR] Entrada invalida en los valores numericos.\n";
            return false;
        }
        arreglo = {v1, v2, v3};

        int cantRegs = leerEntero("Cuantos elementos escolares deseas agregar (alumno, asignatura, aula, equipo, etc.)?: ");
        if (cantRegs < 0) cantRegs = 0;
        registros.clear();

        for (int i = 0; i < cantRegs; ++i) {
            cout << "  --- Registro escolar [" << i + 1 << "] ---\n";
            string tipo = leerPalabra("    Tipo (ej. alumno, asignatura, aula, laboratorio): ");
            string etiqueta = leerLinea("    Nombre o clave del registro: ");
            RegistroEscolar reg(tipo, etiqueta);

            int cantDesc = leerEntero("    Cuantas descripciones deseas anadirle?: ");
            if (cantDesc < 0) cantDesc = 0;
            for (int d = 0; d < cantDesc; ++d) {
                string desc = leerLinea("      Descripcion [" + to_string(d + 1) + "]: ");
                reg.agregarDescripcion(desc);
            }
            registros.push_back(move(reg));
        }

        return true;
    }
}

// ==========================================
// 6. FUNCION PRINCIPAL CON MENU INTERACTIVO
// ==========================================
int main() {
    unique_ptr<IPersistenciaEscuelaCSV> servicioCSV = make_unique<ManejadorEscuelaCSV>();
    GestorEscuela sistema(move(servicioCSV));

    int opcion;
    string nombreArchivo = "escuela_adt.csv";

    do {
        cout << "\n=========================================\n";
        cout << "      MENU - SISTEMA ESCOLAR (CSV)       \n";
        cout << "=========================================\n";
        cout << "1. Agregar nuevo registro escolar (ADT)\n";
        cout << "2. Ver registros en memoria RAM\n";
        cout << "3. Guardar registros en archivo CSV (Disco)\n";
        cout << "4. Cargar registros desde archivo CSV (Disco)\n";
        cout << "5. Editar registro existente en memoria RAM\n";
        cout << "6. Eliminar registro existente en memoria RAM\n";
        cout << "7. Salir\n";
        opcion = leerEntero("Selecciona una opcion: ");

        switch (opcion) {
            case 1: {
                int id;
                string nombre;
                vector<int> arreglo;
                vector<RegistroEscolar> registros;
                if (capturarDatosEscolares(id, nombre, arreglo, registros)) {
                    sistema.agregarElemento(id, nombre, arreglo, move(registros));
                }
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
            case 5: {
                if (sistema.estaVacia()) {
                    cout << "\n[AVISO] No hay registros en memoria RAM para editar.\n";
                    break;
                }
                sistema.listarResumido();
                int num = leerEntero("Selecciona el numero de registro a editar: ");
                if (num < 1 || static_cast<size_t>(num) > sistema.cantidadElementos()) {
                    cout << "\n[ERROR] Numero de registro fuera de rango.\n";
                    break;
                }
                int id;
                string nombre;
                vector<int> arreglo;
                vector<RegistroEscolar> registros;
                if (capturarDatosEscolares(id, nombre, arreglo, registros)) {
                    sistema.editarElemento(
                        static_cast<size_t>(num - 1), id, nombre, arreglo, move(registros)
                    );
                }
                break;
            }
            case 6: {
                if (sistema.estaVacia()) {
                    cout << "\n[AVISO] No hay registros en memoria RAM para eliminar.\n";
                    break;
                }
                sistema.listarResumido();
                int num = leerEntero("Selecciona el numero de registro a eliminar: ");
                if (num < 1 || static_cast<size_t>(num) > sistema.cantidadElementos()) {
                    cout << "\n[ERROR] Numero de registro fuera de rango.\n";
                    break;
                }
                sistema.eliminarElemento(static_cast<size_t>(num - 1));
                break;
            }
            case 7:
                cout << "\nSaliendo del programa. Hasta luego!\n";
                break;
            default:
                cout << "\n[ERROR] Opcion no valida. Intenta de nuevo.\n";
        }
    } while (opcion != 7);

    return 0;
}