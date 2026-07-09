#include "hbapi.h"
#include "hbapiitm.h"

#include "hbdockhb.h"

void * hb_dockParamHandle( int iParam )
{
   if( HB_ISPOINTER( iParam ) )
      return hb_parptr( iParam );

   if( HB_ISNUM( iParam ) )
      return ( void * ) ( HB_PTRDIFF ) hb_parnl( iParam );

   return NULL;
}

void hb_dockRetHandle( void * handle )
{
   hb_retptr( handle );
}
