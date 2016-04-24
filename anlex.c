/*
 *	Analizador Léxico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Práctica de Programación Nro. 1
 *	
 *	Descripcion:
 *	Implementa un analizador léxico que reconoce números, identificadores, 
 * 	palabras reservadas, operadores y signos de puntuación para JSON.
 *	
 */

/*********** Inclusión de cabecera **************/
#include "anlex.h"


/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente json
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea

/**************** Funciones **********************/


// Rutinas del analizador lexico

void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}

void sigLex()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	int caracter=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF)
	{
		
		if (c==' ' || c=='\t')
			continue;	//eliminar espacios en blanco
		else if(c=='\n')
		{
			//incrementar el numero de linea
			numLinea++;
			continue;
		}
		else if (c=='"')
		{
			//elimina el comentario
			i=0;
			while(c!=EOF)
			{
				c=fgetc(archivo);
				id[i]=c;
				i++;
				if (c=='"'){
					id[i-1]='\0';
					break;
				}
				else if(c=='\n')
				{
					//incrementar el numero de linea
					numLinea++;
				}
			}
			if (c==EOF)
				error("Se llego al fin de archivo sin finalizar un comentario");
	
			strcpy(e.lexema,id);
			e.compLex=LITERAL_CADENA;
			insertar(e);
			t.pe=buscar(id);
			t.compLex=LITERAL_CADENA;
			
			break;
		}
		else if (isalpha(c))
		{
			//es un identificador (o palabra reservada)
			i=0;
			do{
				id[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMLEX)
					error("Longitud de Identificador excede tamaño de buffer");
			}while(isalpha(c) || isdigit(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(id);
			t.compLex=t.pe->compLex;
			if (t.pe->compLex==-1)
			{
				strcpy(e.lexema,id);
				e.compLex=LITERAL_NUM;
				insertar(e);
				t.pe=buscar(id);
				t.compLex=LITERAL_NUM;
			}
			break;
		}
		else if (isdigit(c))
		{
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else if (isalpha(c)) {
							id[++i]=c;
							estado = 0;
							caracter =1;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						t.pe=buscar(id);
						if (caracter == 1){
							strcpy(e.lexema,id);
							e.compLex=ERROR;
							insertar(e);
							t.pe=buscar(id);
							t.compLex=ERROR;
							caracter = 0;
						}
						else if (t.pe->compLex==-1)
						{
							strcpy(e.lexema,id);
							e.compLex=LITERAL_NUM;
							insertar(e);
							t.pe=buscar(id);
							t.compLex=LITERAL_NUM;
						}
						
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
						exit(1);
					}
				}
			break;
		}
		
		else if (c==':')
		{
			
				t.compLex=DOS_PUNTOS;
				t.pe=buscar(":");
				break;
			
		}
		
		else if (c==',')
		{
			t.compLex=COMA;
			t.pe=buscar(",");
			break;
		}
		
		
		
		else if (c=='[')
		{
			t.compLex=L_CORCHETE;
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			t.compLex=R_CORCHETE;
			t.pe=buscar("]");
			break;
		}
		
		else if (c=='{')
		{
			t.compLex=L_LLAVE;
			t.pe=buscar("{");
			break;
		}
		else if (c=='}')
		{
			t.compLex=R_LLAVE;
			t.pe=buscar("}");
			break;
		}
		
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.compLex=EOF;
		strcpy(e.lexema,"EOF");
		t.pe=&e;
	}
	
}

int main(int argc,char* args[])
{
	// inicializar analizador lexico

	initTabla();
	initTablaSimbolos();
	
	FILE *salida;
    salida=fopen("output.txt", "w");
    if(salida==NULL)        return 1;
    
    char *tokens[]={"LITERAL_CADENA",
			"LITERAL_NUM",
			"PR_TRUE",
			"PR_FALSE",
			"PR_NULL",
			"L_CORCHETE",
			"R_CORCHETE",
			"L_LLAVE",
			"R_LLAVE",
			"COMA",
			"DOS_PUNTOS",
			"ERROR"};
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		int numLineaAux=1;
		while (t.compLex!=EOF){
			sigLex();
			if(t.compLex==EOF) {
				fprintf(salida,"EOF");
				printf("\t EOF");
				break;
			}
			if(numLineaAux==numLinea){
				fprintf(salida," %s ",tokens[t.compLex]);
				printf("\nLin %d: %s -> %s",numLinea,t.pe->lexema,tokens[t.compLex]);
			}else{
				fprintf(salida,"\n");
				fprintf(salida, " %s ",tokens[t.compLex]);
				printf("\nLin %d: %s -> %s",numLinea,t.pe->lexema,tokens[t.compLex]);
				numLineaAux++;
			}		
		}
	
		fclose(archivo);
	    	fclose(salida);
		printf("\n");

	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
}
