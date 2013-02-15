// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
// Misc utility modules
#include "main.h"

// Struct compare function for QSORT
// [v1.01] should now be memory leak free
int _FcCompareStruct(const void *a, const void *b) 
{ 
	char* cmp1 = (char*)toLowerCase((* (class c_game **) a)->title, 256);
	char* cmp2 = (char*)toLowerCase((* (class c_game **) b)->title, 256);

	int nRet = strcmp(cmp1, cmp2);

	if(cmp1) {	free(cmp1); *&cmp1 = NULL; }
	if(cmp2) {	free(cmp2); *&cmp2 = NULL; }

	return nRet;
}

// String compare function for QSORT
int _FcCompareStr(const void *pStrA, const void *pStrB)
{
	const bool bAscending = true;	// Always ascending [left as reference]
    return bAscending ? 
		strcmp(*(char**)pStrA, *(char**)pStrB):	// Ascending [Ex. A,B,C]
		strcmp(*(char**)pStrB, *(char**)pStrA); // Descending [Ex. C,B,A]
}

#define _ADDCH(str, ch)			\
	str[strlen(str)]	= ch;	\
	str[strlen(str)+1]	= '\0';

char* GetNumWithCommas(int64_t nNumber) 
{
	bool bIsNegative				= false;
	char szNumber[256]				= "";
	char szFlippedNumber[256]		= "";
	char szCommaFlippedNumber[256]	= "";

	// Check negative number
	if(nNumber < 0) {
		bIsNegative = true;
	}

	// Convert integer to string
	sprintf(szNumber, "%lld", nNumber);	

	// Remove negative sign if needed
	if(bIsNegative) {
		char* pszTmp = szNumber + 1;
		sprintf(szNumber, "%s", pszTmp);
	}

	// Flip number string
	for(unsigned int nDigit = 0; nDigit < strlen(szNumber); nDigit++) {
		_ADDCH(szFlippedNumber, szNumber[strlen(szNumber)-(nDigit+1)])
	}	

	// Add commas each 3 digits
	unsigned int nDigit = 0, nCount = 0;

	while(nDigit < strlen(szFlippedNumber))
	{
		if(nCount == 3) {
			_ADDCH(szCommaFlippedNumber, ',')
			nCount = 0;
			continue;
		}
		_ADDCH(szCommaFlippedNumber, szFlippedNumber[nDigit])
		nDigit++;
		nCount++;
	}
	
	// Avoid memory issues
	char* szCommaNumber	= (char*)malloc(strlen(szCommaFlippedNumber)+2); // +2 for the '-' and '\0'

	// Flip the new formatted number
	for(nDigit = 0; nDigit < strlen(szCommaFlippedNumber); nDigit++) {
		_ADDCH(szCommaNumber, szCommaFlippedNumber[strlen(szCommaFlippedNumber)-(nDigit+1)])
	}

	// Add negative sign if needed
	if(bIsNegative) {
		char szNegative[256] = "";
		sprintf(szNegative, "-%s", szCommaNumber);
		strcpy(szCommaNumber, szNegative);
	}

	return szCommaNumber;
}

// Simple function to convert a string to lower case
char* toLowerCase(char* pszStr, size_t nLen)
{	
	char* pszNewStr = NULL; 
	pszNewStr = (char*)malloc(sizeof(char) * nLen+1);
	memcpy(pszNewStr, pszStr, nLen);
	pszNewStr[nLen] = 0; // null-terminated string
		
	for(int ch = 0; pszNewStr[ch]; ch++) {
		pszNewStr[ch] = tolower(pszNewStr[ch]);
	}

	return pszNewStr;
}

// Simple function to convert a string to upper case
char* toUpperCase(char* pszStr, size_t nLen)
{	
	char* pszNewStr = NULL; 
	pszNewStr = (char*)malloc(sizeof(char) * nLen+1);
	memcpy(pszNewStr, pszStr, nLen);
	pszNewStr[nLen] = 0; // null-terminated string
		
	for(int ch = 0; pszNewStr[ch]; ch++) {
		pszNewStr[ch] = toupper(pszNewStr[ch]);
	}

	return pszNewStr;
}

// lpByteSize / 1,024
#define CALCKILOBYTE(lpByteSize)	(double)((double)(lpByteSize) / (1024.0F))
// lpByteSize / 1,048,576
#define CALCMEGABYTE(lpByteSize)	(double)((double)(lpByteSize) / (1024.0F * 1024.0F))
// lpByteSize / 1,073,741,824
#define CALCGIGABYTE(lpByteSize)	(double)((double)(lpByteSize) / (1024.0F * 1024.0F * 1024.0F))
// lpByteSize / 1,099,511,627,776
#define CALCTERABYTE(lpByteSize)	(double)((double)(lpByteSize) / (1024.0F * 1024.0F * 1024.0F * 1024.0F))

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int64_t _ex(int64_t nNum, int64_t nEx)
{
	int64_t nFinalNum = nNum;
	for(int64_t m = 0; m < nEx-1; m++) {
		nFinalNum = nFinalNum * nNum;
	}
	return nFinalNum;
}

double GetByteUnit(int64_t nSize) 
{
	if(nSize >= _ex(1024, 4)) { return CALCTERABYTE(nSize); }
	if(nSize >= _ex(1024, 3)) { return CALCGIGABYTE(nSize); }
	if(nSize >= _ex(1024, 2)) { return CALCMEGABYTE(nSize); }
	if(nSize >= _ex(1024, 1)) { return CALCKILOBYTE(nSize); }
	return (double)nSize;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

const char* GetByteUnitStr(int64_t nSize) 
{	
	if(nSize >= _ex(1024, 4)) { return "TB"; }
	if(nSize >= _ex(1024, 3)) { return "GB"; }
	if(nSize >= _ex(1024, 2)) { return "MB"; }
	if(nSize >= _ex(1024, 1)) { return "KB"; }
	return "Bytes";
}
