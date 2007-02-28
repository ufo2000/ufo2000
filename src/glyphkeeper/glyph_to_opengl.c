/*  OpenGL Add On to Glyph Keeper
 *  by Esa Tanskanen
 *
 */

#ifdef included_from_glyph_c


#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGGL)
   #include <alleggl.h>
#elif (GLYPH_TARGET == GLYPH_TARGET_OPENGL)
   #include <gl/gl.h>
#endif


/* Used to extract the color components from GLYPH_REND::text_alpha_color */
#define GLYPH_GETR( color ) ((color >> 16) & 0xff)
#define GLYPH_GETG( color ) ((color >> 8) & 0xff)
#define GLYPH_GETB( color ) ((color) & 0xff)
#define GLYPH_GETA( color ) ((color >> 24) & 0xff)


typedef unsigned char GLYPH_GL_PIXELTYPE;


/* Texture data storage */

struct GLYPH_TEXTURE {
   
   /* OpenGL "pointers" to the associated texture and display lists */
   
   GLuint textureId;
   
   GLuint displayListStart;
   
   GLuint displayListBase;
   
   /* Glyph information, creates its own copy of the glyphs */
   
   int numGlyphs;
   
   GLYPH **glyphs;
   
   GLYPH_GL_PIXELTYPE *pixeldata;
   
   /* Width and height of the pixel data */
   int w, h;
   
   int splits;
   int glyphsPerSplit;
   int splitHeight;
   
   /* Amount of memory consumed by the texture */
   int neededMemory;
   
   GLYPH_REND *rend;
   
};


/* Several internal function definitions */
GLuint _gk_load_texture( const GLYPH_GL_PIXELTYPE *pixeldata, int w, int h );
int gk_reload_texture( GLYPH_TEXTURE *texture );
void gk_send_texture_to_gpu( GLYPH_TEXTURE *texture );
void _gk_set_orthographic_projection();
void _gk_reset_projection();



static int iabs( int num ) {
   return (num < 0)? -num : num;
}


static int _glyphs_per_line( int numGlyphs, int splitCount ) {
   return numGlyphs/splitCount+1;
}



/*  
 *  Creates the CLYPH_TEXTURE object, but doesn't
 *  output anything to OpenGL or the gfx card
 */

GLYPH_TEXTURE *_gk_glyphs_to_texture( GLYPH_REND *rend, GLYPH **glyphs, int numGlyphs, GLuint displayListBase )
{
   
   GLYPH_TEXTURE *texture = 0;
   int totalWidth = 0;
   int maxHeight = 0;
   
   int glyphx;
   //int x;
   int y;
   unsigned char *srcptr;
   GLYPH_GL_PIXELTYPE *dstptr;
   int index;
   
   int splitCount;
   int chosenSplitcount;
   int minDiff;
   int currentDiff;
   int line;
   int maxiumWidth;
   int currentWidth;
   int currentLineEnd;
   int textureW;
   int textureH;
   int lineBaseIndex;
   
   CARE( first );
   CARE( last );


   texture = (GLYPH_TEXTURE *) malloc( sizeof( GLYPH_TEXTURE ));
   
   texture->displayListBase = displayListBase;
   texture->textureId = -1;
   texture->numGlyphs = numGlyphs;
   texture->glyphs = glyphs;
   texture->rend = rend;
   
   /* Find the width and height of the texture */
   
   for( index = 0; index < numGlyphs; index++ ) {
      totalWidth += glyphs[index]->width;
      
      if( glyphs[index]->height > maxHeight )
         maxHeight = glyphs[index]->height;
   }
   
   
   /* Find the smallest texture size */
   
   texture->w = 1;
   texture->h = 1;
   
   while( texture->w < totalWidth ) texture->w <<= 1;
   while( texture->h < maxHeight ) texture->h <<= 1;
   
   
   splitCount = 0;
   chosenSplitcount = splitCount;
   minDiff = iabs( texture->w - texture->h );
   
   
   do {
      ++splitCount;
      
      if( numGlyphs/splitCount < 10 ) {
         break;
      }
      
      maxiumWidth = 0;
      index = 0;
      
      for( line = 0; line < splitCount; line++ ) {
         currentWidth = 0;
         currentLineEnd = index + _glyphs_per_line( numGlyphs, splitCount );
         for(; index < currentLineEnd && index < numGlyphs; index++ ) {
            currentWidth += glyphs[index]->width;
         }
         
         if( currentWidth > maxiumWidth ) {
            maxiumWidth = currentWidth;
         }
      }
      
      
      if( maxiumWidth < maxHeight * splitCount/2 ) {
         break;
      }
      
      textureW = 1;
      textureH = 1;
      
      while( textureW < maxiumWidth ) textureW <<= 1;
      while( textureH < maxHeight * splitCount ) textureH <<= 1;
      
      currentDiff = iabs( textureW - textureH );
      
      
      if( currentDiff < minDiff ) {
         minDiff = currentDiff;
         chosenSplitcount = splitCount;
         texture->w = textureW;
         texture->h = textureH;
      }
      
   }
   while( 1 );
   
   
   if( chosenSplitcount > 0 ) {
      texture->glyphsPerSplit = _glyphs_per_line( numGlyphs, chosenSplitcount );
   }
   else {
      texture->glyphsPerSplit = numGlyphs;
   }
   
   texture->splits = chosenSplitcount;
   texture->splitHeight = maxHeight;
   
   
   texture->neededMemory = texture->w * texture->h * sizeof( GLYPH_GL_PIXELTYPE )*100;
   
   /* Copy the pixel data */
   
   texture->pixeldata = (GLYPH_GL_PIXELTYPE *) malloc( texture->neededMemory );
   
   memset( texture->pixeldata, 0, texture->neededMemory );
   
   CARE( texture->pixeldata );
   
   index = 0;
   
   /* Copy glyph bitmap data to the texture buffer */
   for( line = 0; line < texture->splits; line++ ) {
      glyphx = 0;
      currentLineEnd = index + _glyphs_per_line( numGlyphs, texture->splits );
      lineBaseIndex = line * texture->splitHeight * texture->w;
      
      for(; index < currentLineEnd && index < numGlyphs; index++ ) {
         srcptr = glyphs[index]->bmp+1;
         for( y = 0; y < glyphs[index]->height; y++ ) {
            /* Copy one line of the glyph to the texture buffer */
            dstptr = lineBaseIndex + texture->pixeldata + y * texture->w + glyphx;
            memcpy( dstptr, srcptr, glyphs[index]->width );
            srcptr += glyphs[index]->width;
         }
         
         glyphx += glyphs[index]->width;
      }
   }
   
   return texture;
}



/*  
 *  Creates the display lists which is later used
 *  to output the glyphs to the screen
 */

void _gk_create_display_lists( GLYPH_TEXTURE *texture )
{
   int index;
   int textureX = 0;
   int textureY = 0;
   double textureCoX;
   double textureCoY;
   double textureW, textureH;
   int line = 0;
   int lineEnd = texture->glyphsPerSplit;
   GLYPH *currentGlyph;
   
   CARE( texture );
   
   texture->displayListStart = glGenLists( texture->numGlyphs );
   
   for( index = 0; index < texture->numGlyphs; index++ ) {
      if( index >= lineEnd ) {
         lineEnd += texture->glyphsPerSplit;
         line++;
         textureX = 0;
         textureY += texture->splitHeight;
      }
      
      currentGlyph = texture->glyphs[index];
      
      textureCoX = (double) textureX/texture->w;
      textureW = (double) currentGlyph->width/texture->w;
      
      textureCoY = (double) textureY/texture->h;
      textureH = (double) currentGlyph->height/texture->h;
      
      
      glNewList( texture->displayListStart + index, GL_COMPILE );
         /* Adjust the quad position according to the anchor */
         
         glTranslated( currentGlyph->left, -currentGlyph->top, 0 );
         
         glBegin( GL_QUADS );
            /* Bottom-left */
            glTexCoord2f( textureCoX, textureCoY + textureH );
            
            glVertex2i( 0, currentGlyph->height );
            
            /* Bottom-right */
            glTexCoord2f( textureCoX + textureW, textureCoY + textureH );
            
            glVertex2i( currentGlyph->width, currentGlyph->height );
            
            /* Top-right */
            glTexCoord2f( textureCoX + textureW, textureCoY );
            
            glVertex2i( currentGlyph->width, 0 );
            
            /* Top-left */
            glTexCoord2f( textureCoX, textureCoY );
            
            glVertex2i( 0, 0 );
            
         glEnd();
         
         /* Revert the anchor displacement and advance the orign such that */
         /* the next glyph drawn at the correct place */
         glTranslated( -currentGlyph->left + (double) (currentGlyph->advance_x)/(double) (1<<6), currentGlyph->top - (double) (currentGlyph->advance_y)/(double) (1<<6), 0 );
         
      glEndList();
      
      textureX += currentGlyph->width;
   }
};




/*
 *  Creates the texture using a GLYPH_REND
 */

GLYPH_TEXTURE *gk_create_texture( GLYPH_REND *rend, int rangeStart, int rangeLength )
{
   GLYPH_TEXTURE *texture = 0;
   GLYPH **glyphs = 0;
   GLYPH *returned = 0;
   
   int index;
   
   CARE( rend );
   CARE( rangeStart > 0 && rangeStart + rangeLength <= 255 );
   
   glyphs = (GLYPH **) malloc( sizeof( GLYPH* ) * rangeLength );
   
   for( index = 0; index < rangeLength; index++ ) {
      returned = _gk_rend_render( rend, rangeStart + index );
      
      /* Create a copy of the glyph so that we don't have to care of what happens to the original */
      
      glyphs[index] = returned;//(GLYPH *) malloc( sizeof( GLYPH ));
      //memcpy( glyphs[index], returned, sizeof( GLYPH ));
   }
   
   texture = _gk_glyphs_to_texture( rend, glyphs, rangeLength, rangeStart );
   
   CARE( texture );
   
   _gk_create_display_lists( texture );
   
   return texture;
}



void gk_destroy_texture( GLYPH_TEXTURE *texture )
{
   int i;
   CARE( texture );
   
   if( texture->textureId != 0 )
      gk_unload_texture_from_gpu( texture );
   
   for( i = 0; i < texture->numGlyphs; i++ ) {
      free( texture->glyphs[i] );
   }
   
   free( texture->glyphs );
   free( texture->pixeldata );
   free( texture );
}



/*
 *  Renders a display list from a texture, printing the text to screen
 *  This function doesn't take a top-left position for the text, as it
 *  should already be set in the modelview matrix
 */

void _gk_render_display_list( const GLYPH_TEXTURE *texture, const char *text )
{
   CARE( texture );
   
   /* Store the user display list settings so that we don't mess them up */
   glPushAttrib( GL_LIST_BIT );
   
   /* Select the base of character rendering */
   glListBase( texture->displayListStart - texture->displayListBase );
   
   /* Send the display lists to gfx card according to our text input */
   glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );
   
   /* Restore the previous display list settings */
   glPopAttrib();
}



/*
 *  Renders the text to screen
 */

void gk_render_line_gl_utf8( GLYPH_TEXTURE *texture, const char *text, int x, int y )
{
   int textColor;
   
   CARE( texture );
   
   if( texture->textureId == (unsigned)-1 )
      gk_send_texture_to_gpu( texture );
   
   _gk_set_orthographic_projection();
   
   glEnable( GL_TEXTURE_2D );
   glBindTexture( GL_TEXTURE_2D, texture->textureId );
   
   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
   
   glPushMatrix();
   
   glTranslated( x, y, 0.0 );
   
   textColor = texture->rend->text_alpha_color;
    
   glColor4f( GLYPH_GETR( textColor )/255.0, GLYPH_GETG( textColor )/255.0, GLYPH_GETB( textColor )/255.0, GLYPH_GETA( textColor )/255.0 );
   
   _gk_render_display_list( texture, text );
   
   glPopMatrix();
   
   _gk_reset_projection();
}



/* 
 * Load or reload the texture to the gfx card
 */
 
void gk_send_texture_to_gpu( GLYPH_TEXTURE *texture )
{
   CARE( texture );
   
   texture->textureId = _gk_load_texture( texture->pixeldata, texture->w, texture->h );
}


/* 
 * Unload the texture in the gfx card
 */

void gk_unload_texture_from_gpu( GLYPH_TEXTURE *texture )
{
   CARE( texture );
   
   glDeleteTextures( 1, &texture->textureId );
   
   texture->textureId = 0;
}


/* Load bitmap information to gfx card as a texture, returns the texture id */

GLuint _gk_load_texture( const GLYPH_GL_PIXELTYPE *pixeldata, int w, int h )
{
   GLuint textureId;
   
#if (GLYPH_TARGET == GLYPH_TARGET_ALLEGGL)

   BITMAP *temp;
   int i;
   
   /* Unfortunately, we currenly need to make a copy of the pixel data */
   
   temp = create_bitmap_ex( 8, w, h );
   
   for( i = 0; i < w*h; i++ )
      putpixel( temp, i%w, i/w, pixeldata[i] );
   
   allegro_gl_use_alpha_channel( TRUE );
   allegro_gl_use_mipmapping(FALSE);
   allegro_gl_set_texture_format( GL_ALPHA8 );
   
   textureId = allegro_gl_make_texture_ex( AGL_TEXTURE_ALPHA_ONLY, temp, GL_ALPHA8 );
   
   destroy_bitmap( temp );

#else

   glGenTextures( 1, &textureId );
   glBindTexture( GL_TEXTURE_2D, textureId );
   glTexImage2D( GL_TEXTURE_2D, 0, 1, w, h, 0, GL_ALPHA8, GL_UNSIGNED_BYTE, pixeldata );
   
#endif
   
   return textureId;
}



/* Selects orthographic projection to allow pixel-based placements */

void _gk_set_orthographic_projection()
{
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( 0, SCREEN_W, 0, SCREEN_H, -1, 1000 );
    glScalef( 1, -1, 1 );
    glTranslatef( 0, -SCREEN_H, 0 );
    glMatrixMode( GL_MODELVIEW );
    glDisable(GL_DEPTH_TEST);
}



/*  Resets the previous projection matrix, stored by _gk_set_orthographic_projection */

void _gk_reset_projection()
{
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
}



/* The following Glyph Keeper functions are currently defined empty in this context */

static void _gk_prepare_to_draw(GLYPH_TARGET_SURFACE* const new_bmp,GLYPH_REND* const new_rend) {}
static void _gk_put_glyph(GLYPH* const glyph,const int x,const int y) {}
static void _gk_done_drawing() {}




#endif  /* included_from_glyph_c */