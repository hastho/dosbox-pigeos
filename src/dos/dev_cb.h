#include <windows.h>
#include "dos_inc.h"
#include "timer.h"

class device_CB : public DOS_Device {
public:
	device_CB () { 
		SetName ("CLPBOARD"); 
		Handle = NULL;
	};

	bool Read (Bit8u * data, Bit16u * size) { 
		HGLOBAL		hc;
		LPTSTR		*p; 
		size_t		len;

		if (!Handle) {
			if (!(Handle = tmpfile ()))
				return false;

			if (!OpenClipboard (NULL)) {
				fclose (Handle);
				Handle = NULL;
				return false;
			}

			if (!(hc = GetClipboardData (CF_TEXT))) {
				*size = 0;
				CloseClipboard ();
				return false;
			}

			p = (LPTSTR *)(GlobalLock (hc)); 
			len = strlen ((char *)(p)) * sizeof( TCHAR);

#ifdef SYNCGW_FILECHAR
			ConvertGerman ((char *)(p), false);
#endif
			fwrite (p, sizeof (Bit8u), len, Handle);
			fseek (Handle, 0, SEEK_SET);

			GlobalUnlock (hc); 
			CloseClipboard ();
		} 
		
		if ((*size = fread (data, sizeof (Bit8u), 32768, Handle)) < 32768) {
			fclose (Handle);
			Handle = NULL;
		}

		return true; 
	};
	bool Write (Bit8u * data, Bit16u * size) {

		if (!Handle) {
			if (!(Handle = tmpfile ()))
				return false;
		}

#ifdef SYNCGW_FILECHAR
		ConvertGerman((char *)(data), true);
#endif

		fwrite (data, sizeof (Bit8u), *size, Handle);

		return true;
	};
	
	bool Seek (Bit32u * pos, Bit32u type) { 
		return true; 
	};
	
	bool Close () { 
		HGLOBAL		hc; 
		Bit8u		*p;
		size_t		len;

		if (Handle) {
			fseek (Handle, 0, SEEK_END);
			len = ftell (Handle);
			fseek (Handle, 0, SEEK_SET);

			if (!OpenClipboard (NULL)) {
				fclose (Handle);
				Handle = NULL;
				return false;
			}
			EmptyClipboard ();

			hc = GlobalAlloc (GMEM_MOVEABLE, (len + 1) * sizeof (Bit8u)); 
			p = (Bit8u *)(GlobalLock (hc)); 
			fread (p, sizeof (Bit8u), len, Handle);
			p[len] = (Bit8u)(0);

			GlobalUnlock (hc);
			SetClipboardData (CF_TEXT, hc);
			CloseClipboard ();
	
			fclose (Handle);
			Handle = NULL;
		}

		return true; 
	};
	
	Bit16u GetInformation (void) { 
		return 0x8000; 
	}; 

private:
	FILE		*Handle;
};
