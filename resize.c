/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 * By Jacob Sherman  jacob@jbsherman.com
 * Resize a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"


int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize number infile outfile\n");
        return 1;
    }

    // remember filenames
    char* numChar = argv[1];
    char* infile = argv[2];
    char* outfile = argv[3];
    
    //  convert char resize number input to a int
    int n = atoi( numChar);

    if (n < 1 || n > 100)
    {
        printf("n, the resize factor, must satisfy 0 < n <=100.\n");
        return 11;
    }
    

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // STARTING  original copy code determine padding for scanlines
    int Startpadding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // save needed starting values in new variables before overwriting struct
    int startHeight = bi.biHeight;
    int startWidth = bi.biWidth;

// determine new amount of pixels per line and padding
    // biWidth is number of pixels NOT including padding
    int newWidth = bi.biWidth * n;
    
    // padding sizeof returns bytes
    int newPadding = (4 - (newWidth *  sizeof(RGBTRIPLE)) % 4) % 4;
    
    // new height does not need to know padding
    int newHeight = bi.biHeight * n;
    
    // line size with padding  newPadding in bytes + newWidth pixesl times 3 for bytes
    int newLineSize = (newWidth * 3) + newPadding;
    
// determine total size of color table  need to consider width AND height in bytes
    int newSizeImage = newLineSize * abs(newHeight);

// save initial values of changed values
 //   
 //   
 //   int startFileSize = bf.bfSize;
 //   int startImageSize = bi.biSizeImage;

// change values read into buffer from inptr
    bi.biWidth = newWidth;
    bi.biHeight = newHeight;
    bi.biSizeImage = newSizeImage;
    bf.bfSize = 54 + newSizeImage;
    
//////////////////////////////////////////  IO OPERATIONS START  ////////////////////
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
   for (int i = 0; i < abs(startHeight); i++)
   {
        // factor each scanline vertical
        for (int resizeVert = 0; resizeVert < n; resizeVert++)
        {
    
                // iterate over pixels in scanline-  horizontal factor
                for (int j = 0; j < startWidth; j++)
                {
                    // temporary storage
                    RGBTRIPLE triple;

                    // read RGB triple from infile
                    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                    // resize loop based on factor
                        for (int z = 0; z < n; z++)
                        {
                            // write RGB triple to outfile
                            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                        
                        }
                }   /////////////     per pixel factoring horizontal
                
                //  add padding to line if any
                if (newPadding != 0)
                {
                    for (int c = 0; c < newPadding; c++)
                    {
                        fputc(0x00,outptr);
                     }
                }
                 //  seek back to beginning of inptr 
                fseek(inptr, -(startWidth * 3), SEEK_CUR);
           }                 
        // skip over inptr line and padding, if any
        fseek(inptr, ((startWidth * 3) + Startpadding), SEEK_CUR);
    }
    
    // close infile
    fclose(inptr);
    // close outfile
    fclose(outptr);
    // that's all folks
    return 0;
}
