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
// UTILIDAD DE TEXTO Y ESCAPE XML (SRP)
// ==========================================
class UtilXML {
public:
    static string limpiar(const string& s) {
        size_t ini = s.find_first_not_of(" \t\r\n");
        if (ini == string::npos) return "";
        size_t fin = s.find_last_not_of(" \t\r\n");
        return s.substr(ini, fin - ini + 1);
    }

    static bool parsearEnteroSeguro(const string& texto, int& salida) {
        string limp = limpiar(texto);
        if (limp.empty()) return false;
        try {
            size_t idx = 0;
            int val = stoi(limp, &idx);
            if (idx != limp.size()) return false;
            salida = val;
            return true;
        } catch (...) {
            return false;
        }
    }

    static string escaparXML(const string& texto) {
        string out;
        out.reserve(texto.size());
        for (char c : texto) {
            if (c == '&') out += "&amp;";
            else if (c == '<') out += "&lt;";
            else if (c == '>') out += "&gt;";
            else if (c == '"') out += "&quot;";
            else if (c == '\'') out += "&apos;";
            else out += c;
        }
        return out;
    }

    static string desescaparXML(const string& texto) {
        string out;
        out.reserve(texto.size());
        for (size_t i = 0; i < texto.size(); ++i) {
            if (texto[i] == '&') {
                if (texto.compare(i, 5, "&amp;") == 0) {
                    out += '&';
                    i += 4;
                } else if (texto.compare(i, 4, "&lt;") == 0) {
                    out += '<';
                    i += 3;
                } else if (texto.compare(i, 4, "&gt;") == 0) {
                    out += '>';
                    i += 3;
                } else if (texto.compare(i, 6, "&quot;") == 0) {
                    out += '"';
                    i += 5;
                } else if (texto.compare(i, 6, "&apos;") == 0) {
                    out += '\'';
                    i += 5;
                } else {
                    out += texto[i];
                }
            } else {
                out += texto[i];
            }
        }
        return out;
    }

    static string extraerTag(const string& bloque, const string& tag) {
        string apertura = "<" + tag + ">";
        string cierre = "</" + tag + ">";
        size_t pos1 = bloque.find(apertura);
        if (pos1 == string::npos) return "";
        size_t inicio = pos1 + apertura.size();
        size_t pos2 = bloque.find(cierre, inicio);
        if (pos2 == string::npos) return "";
        return desescaparXML(bloque.substr(inicio, pos2 - inicio));
    }
};

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

public:
    RegistroEscolar(string t = "asignatura",
                    string etq = "sin_etiqueta",
                    vector<string> descs = {})
        : tipo(move(t)), etiqueta(move(etq)), descripciones(move(descs)) {}

    const string& getTipo() const { return tipo; }
    const string& getEtiqueta() const { return etiqueta; }
    const vector<string>& getDescripciones() const { return descripciones; }

    void agregarDescripcion(const string& desc) {
        string limp = UtilXML::limpiar(desc);
        if (!limp.empty()) {
            descripciones.push_back(limp);
        }
    }

    string aXML(int indent = 12) const {
        string pad(indent, ' ');
        string inner(indent + 4, ' ');
        string descPad(indent + 8, ' ');

        string tipoFinal = UtilXML::limpiar(tipo).empty() ? "asignatura" : UtilXML::limpiar(tipo);
        string etqFinal = UtilXML::limpiar(etiqueta).empty() ? "sin_etiqueta" : UtilXML::limpiar(etiqueta);

        string xml = pad + "<registro_escolar>\n";
        xml += inner + "<tipo>" + UtilXML::escaparXML(tipoFinal) + "</tipo>\n";
        xml += inner + "<etiqueta>" + UtilXML::escaparXML(etqFinal) + "</etiqueta>\n";
        xml += inner + "<descripciones>\n";
        for (const auto& d : descripciones) {
            xml += descPad + "<descripcion>" + UtilXML::escaparXML(d) + "</descripcion>\n";
        }
        xml += inner + "</descripciones>\n";
        xml += pad + "</registro_escolar>\n";
        return xml;
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

    string aXML() const {
        string xml = "    <elemento_escolar>\n";
        xml += "        <id>" + to_string(id) + "</id>\n";
        xml += "        <nombre>" + UtilXML::escaparXML(nombre) + "</nombre>\n";

        xml += "        <calificaciones>\n";
        for (int c : calificaciones) {
            xml += "            <calificacion>" + to_string(c) + "</calificacion>\n";
        }
        xml += "        </calificaciones>\n";

        xml += "        <registros_escolares>\n";
        if (ptrRegistros != nullptr) {
            for (const auto& reg : *ptrRegistros) {
                xml += reg.aXML(12);
            }
        }
        xml += "        </registros_escolares>\n";

        xml += "    </elemento_escolar>\n";
        return xml;
    }
};

// ==========================================
// 3. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaEscuelaXML {
public:
    virtual void escribir(const string& ruta, const vector<shared_ptr<ElementoEscolarADT>>& elementos) = 0;
    virtual vector<shared_ptr<ElementoEscolarADT>> leer(const string& ruta) = 0;
    virtual ~IPersistenciaEscuelaXML() = default;
};

// ==========================================
// 4. IMPLEMENTACION CONCRETA - LSP / SRP
// ==========================================
class ManejadorEscuelaXML : public IPersistenciaEscuelaXML {
private:
    static RegistroEscolar parsearRegistroEscolar(const string& bloque) {
        string tipo = UtilXML::extraerTag(bloque, "tipo");
        if (tipo.empty()) tipo = "asignatura";

        string etiqueta = UtilXML::extraerTag(bloque, "etiqueta");
        if (etiqueta.empty()) etiqueta = "sin_etiqueta";

        RegistroEscolar reg(tipo, etiqueta);

        string bloqueDesc = UtilXML::extraerTag(bloque, "descripciones");
        size_t pos = 0;
        while ((pos = bloqueDesc.find("<descripcion>", pos)) != string::npos) {
            size_t fin = bloqueDesc.find("</descripcion>", pos);
            if (fin == string::npos) break;
            string texto = UtilXML::desescaparXML(
                bloqueDesc.substr(pos + 13, fin - (pos + 13))
            );
            reg.agregarDescripcion(texto);
            pos = fin + 14;
        }

        return reg;
    }

public:
    void escribir(const string& ruta, const vector<shared_ptr<ElementoEscolarADT>>& elementos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo XML para escritura en: " << ruta << "\n";
            return;
        }

        archivo << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        archivo << "<sistema_escolar>\n";
        for (const auto& elem : elementos) {
            archivo << elem->aXML();
        }
        archivo << "</sistema_escolar>\n";

        archivo.close();
        cout << "\n[EXITO] Datos escolares guardados correctamente en: " << ruta << "\n";
    }

    vector<shared_ptr<ElementoEscolarADT>> leer(const string& ruta) override {
        vector<shared_ptr<ElementoEscolarADT>> elementos;
        ifstream archivo(ruta);

        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro un archivo XML previo en la ruta. Se iniciara vacio.\n";
            return elementos;
        }

        stringstream buffer;
        buffer << archivo.rdbuf();
        archivo.close();
        string contenido = buffer.str();

        size_t posElem = 0;
        while ((posElem = contenido.find("<elemento_escolar>", posElem)) != string::npos) {
            size_t finElem = contenido.find("</elemento_escolar>", posElem);
            if (finElem == string::npos) break;

            string bloqueElem = contenido.substr(posElem + 18, finElem - (posElem + 18));

            int id = 0;
            if (!UtilXML::parsearEnteroSeguro(UtilXML::extraerTag(bloqueElem, "id"), id)) {
                posElem = finElem + 19;
                continue;
            }

            string nombre = UtilXML::extraerTag(bloqueElem, "nombre");

            vector<int> cals;
            string bloqueCals = UtilXML::extraerTag(bloqueElem, "calificaciones");
            size_t posCal = 0;
            while ((posCal = bloqueCals.find("<calificacion>", posCal)) != string::npos) {
                size_t finCal = bloqueCals.find("</calificacion>", posCal);
                if (finCal == string::npos) break;
                int val = 0;
                if (UtilXML::parsearEnteroSeguro(
                        bloqueCals.substr(posCal + 14, finCal - (posCal + 14)), val)) {
                    cals.push_back(val);
                }
                posCal = finCal + 15;
            }

            vector<RegistroEscolar> regs;
            string bloqueRegs = UtilXML::extraerTag(bloqueElem, "registros_escolares");
            size_t posReg = 0;
            while ((posReg = bloqueRegs.find("<registro_escolar>", posReg)) != string::npos) {
                size_t finReg = bloqueRegs.find("</registro_escolar>", posReg);
                if (finReg == string::npos) break;
                regs.push_back(parsearRegistroEscolar(
                    bloqueRegs.substr(posReg + 18, finReg - (posReg + 18))
                ));
                posReg = finReg + 19;
            }

            elementos.push_back(
                make_shared<ElementoEscolarADT>(id, move(nombre), move(cals), move(regs))
            );
            posElem = finElem + 19;
        }

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
    unique_ptr<IPersistenciaEscuelaXML> persistencia;

public:
    GestorEscuela(unique_ptr<IPersistenciaEscuelaXML> pers) : persistencia(move(pers)) {}

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
    unique_ptr<IPersistenciaEscuelaXML> servicioXML = make_unique<ManejadorEscuelaXML>();
    GestorEscuela sistema(move(servicioXML));

    int opcion;
    string nombreArchivo = "escuela_adt.xml";

    do {
        cout << "\n=========================================\n";
        cout << "      MENU - SISTEMA ESCOLAR (XML)       \n";
        cout << "=========================================\n";
        cout << "1. Agregar nuevo registro escolar (ADT)\n";
        cout << "2. Ver registros en memoria RAM\n";
        cout << "3. Guardar registros en archivo XML (Disco)\n";
        cout << "4. Cargar registros desde archivo XML (Disco)\n";
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