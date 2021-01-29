#include "amx/amx.h"
#include <ctype.h> 
#include "format.h"
#include <iostream>

//----------------------------------------------------------------------------------

int AMXAPI aux_LoadProgram(AMX* amx, char* filename)
{
	FILE* fp;
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		return AMX_ERR_NOTFOUND;
	}
	AMX_HEADER hdr;
	fread(&hdr, sizeof hdr, 1, fp);
	amx_Align16(&hdr.magic);
	amx_Align32((uint32_t*)&hdr.size);
	amx_Align32((uint32_t*)&hdr.stp);

	if (hdr.magic != AMX_MAGIC)
	{
		fclose(fp);
		return AMX_ERR_FORMAT;
	}

	void* memblock;
	if ((memblock = malloc(hdr.stp)) == NULL)
	{
		fclose(fp);
		return AMX_ERR_MEMORY;
	}

	rewind(fp);
	fread(memblock, 1, (size_t)hdr.size, fp);
	fclose(fp);

	//amx_SetDebugHook(amx, (AMX_DEBUG)amx_Debug);
	memset(amx, 0, sizeof(*amx));
	int result = amx_Init(amx, memblock);
	if (result != AMX_ERR_NONE)
	{
		free(memblock);
		amx->base = NULL;
	}
	return result;
}

//----------------------------------------------------------------------------------

int AMXAPI aux_LoadProgramFromMemory(AMX* amx, char* filedata)
{
	AMX_HEADER hdr;

	memcpy(&hdr, filedata, sizeof hdr);

	amx_Align16(&hdr.magic);
	amx_Align32((uint32_t*)&hdr.size);
	amx_Align32((uint32_t*)&hdr.stp);

	if (hdr.magic != AMX_MAGIC)
	{
		return AMX_ERR_FORMAT;
	}

	void* memblock;
	if ((memblock = malloc(hdr.stp)) == NULL)
	{
		return AMX_ERR_MEMORY;
	}

	memcpy(memblock, filedata, (size_t)hdr.size);

	//amx_SetDebugHook(amx, (AMX_DEBUG)amx_Debug);
	memset(amx, 0, sizeof(*amx));
	int result = amx_Init(amx, memblock);
	if (result != AMX_ERR_NONE)
	{
		free(memblock);
		amx->base = NULL;
	}
	return result;
}
//----------------------------------------------------------------------------------

int AMXAPI aux_FreeProgram(AMX* amx)
{
	if (amx->base != NULL)
	{
		amx_Cleanup(amx);
		free(amx->base);
		memset(amx, 0, sizeof(AMX));
	}
	return AMX_ERR_NONE;
}

//----------------------------------------------------------------------------------

const char* AMXAPI aux_StrError(int errnum)
{
	static char* messages[] = {
		/* AMX_ERR_NONE      */ (char*)"(none)",
		/* AMX_ERR_EXIT      */ (char*)"Forced exit",
		/* AMX_ERR_ASSERT    */ (char*)"Assertion failed",
		/* AMX_ERR_STACKERR  */ (char*)"Stack/heap collision (insufficient stack size)",
		/* AMX_ERR_BOUNDS    */ (char*)"Array index out of bounds",
		/* AMX_ERR_MEMACCESS */ (char*)"Invalid memory access",
		/* AMX_ERR_INVINSTR  */ (char*)"Invalid instruction",
		/* AMX_ERR_STACKLOW  */ (char*)"Stack underflow",
		/* AMX_ERR_HEAPLOW   */ (char*)"Heap underflow",
		/* AMX_ERR_CALLBACK  */ (char*)"No (valid) native function callback",
		/* AMX_ERR_NATIVE    */ (char*)"Native function failed",
		/* AMX_ERR_DIVIDE    */ (char*)"Divide by zero",
		/* AMX_ERR_SLEEP     */ (char*)"(sleep mode)",
		/* 13 */                (char*)"(reserved)",
		/* 14 */                (char*)"(reserved)",
		/* 15 */                (char*)"(reserved)",
		/* AMX_ERR_MEMORY    */ (char*)"Out of memory",
		/* AMX_ERR_FORMAT    */ (char*)"Invalid/unsupported P-code file format",
		/* AMX_ERR_VERSION   */ (char*)"File is for a newer version of the AMX",
		/* AMX_ERR_NOTFOUND  */ (char*)"File or function is not found",
		/* AMX_ERR_INDEX     */ (char*)"Invalid index parameter (bad entry point)",
		/* AMX_ERR_DEBUG     */ (char*)"Debugger cannot run",
		/* AMX_ERR_INIT      */ (char*)"AMX not initialized (or doubly initialized)",
		/* AMX_ERR_USERDATA  */ (char*)"Unable to set user data field (table full)",
		/* AMX_ERR_INIT_JIT  */ (char*)"Cannot initialize the JIT",
		/* AMX_ERR_PARAMS    */ (char*)"Parameter error",
	};
	if (errnum < 0 || errnum >= sizeof messages / sizeof messages[0])
		return "(unknown)";
	return messages[errnum];
}

//----------------------------------------------------------------------------------

cell* get_amxaddr(AMX* amx, cell amx_addr)
{
	return (cell*)(amx->base + (int)(((AMX_HEADER*)amx->base)->dat + amx_addr));
}

//----------------------------------------------------------------------------------

int set_amxstring(AMX* amx, cell amx_addr, const char* source, int max)
{
	cell* dest = (cell*)(amx->base + (int)(((AMX_HEADER*)amx->base)->dat + amx_addr));
	cell* start = dest;
	while (max-- && *source)
		*dest++ = (cell)*source++;
	*dest = 0;
	return dest - start;
}

//----------------------------------------------------------------------------------

char* format_amxstring(AMX* amx, cell* params, int parm, int& len)
{
	static char outbuf[4096];
	cell* addr = get_amxaddr(amx, params[parm++]);
	len = atcprintf(outbuf, sizeof(outbuf) - 1, addr, amx, params, &parm);
	return outbuf;
}
//----------------------------------------------------------------------------------
void logprintf(char* format, ...);
void AMXPrintError(AMX* amx, int error)
{
	if (error != AMX_ERR_NONE)
	{
		logprintf((char*)"Script: Run time error %d: \"%s\"",
			error, aux_StrError(error));
	}
}