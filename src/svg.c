/**
 * @file svg.c
 * @brief Implementation of simple SVG drawing interface.
 *
 * Implements the basic functions for creating SVG documents.
 */
#include "svg.h"
#include <stdlib.h>
#include <stdio.h>


/**
 * @brief Opaque SVG drawing context.
 *
 * Holds the necessary data to implement functions.
 */
struct SVG_CONTEXT{
    svg_write_fn write_fn;
    svg_cleanup_fn cleanup_fn;
    svg_user_context_ptr user;
};


svg_context_ptr svg_create(svg_write_fn write_fn, 
                           svg_cleanup_fn cleanup_fn, 
                           svg_user_context_ptr user, 
                           svg_px_t width, 
                           svg_px_t height){
    svg_context_ptr context = (svg_context_ptr)malloc(sizeof(svg_context_t));

    // Check if all parameters work
    if (!write_fn || !user || width < 0 || height < 0) {
        return NULL;
    }

    context->write_fn = write_fn;
    context->cleanup_fn = cleanup_fn;
    context->user = user;

    // Write the first line of XML to our file (i think an encoding tag?)
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

 // Use the enumeration SVG_OK to check if the function is able to be written
    if (write_fn(user, buffer) != SVG_OK) {
        free(context);
        return NULL;
    }

    // Write the second line of XML to our file (the opening SVG tag that creates the drawing)
    snprintf(buffer, sizeof(buffer), "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\">\n", width, height);

    // Use the enumeration SVG_OK to check if the function is able to be written
    if (write_fn(user, buffer) != SVG_OK) {
        free(context);
        return NULL;
    }

    return context;
}

/*
THE STRUCTURE FOR CREATING ALL THESE FUNCTIONS: 
Check for NULL
Format XML string with snprintf
Write using context->write_fn(context->user, buffer)
Return the result */ 

svg_return_t svg_destroy(svg_context_ptr context){
    // Enter code here
    // Checks if context is NULL
    if (!context) {
        return SVG_ERR_NULL;
    }
    
    // Adding a closing </svg> tag
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "</svg>\n");
   
    // Create the result instead of returning it beacuse you need to free it, you can't free after you return
    svg_return_t result = context->write_fn(context->user, buffer);

     if (context->cleanup_fn) {
        svg_return_t cleanup_result = context->cleanup_fn(context->user);
        // If cleanup fails, return that error instead
        if (cleanup_result != SVG_OK) {
            free(context);
            return cleanup_result;
        }
    }

    // Free then return the output back to the buffer
     free(context);

     return result;
    
}

svg_return_t svg_circle(svg_context_ptr context,
                        const svg_point_t *center,
                        svg_real_t radius,
                        const char *style){
    // Enter code here

    // Checks if context is NULL
    if (!context) {
        return SVG_ERR_NULL;
    }
    if(!center) {
        return SVG_ERR_NULL;
    }
    
    // Creating the buffer for the output
    char buffer[256];
    // Using conditional to deal with NULL styles and outputting the XML function through the buffer with all it's components
    if(style) {
        snprintf(buffer, sizeof(buffer), "<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\"%s\"/>\n", center->x, center->y, radius, style);
    }
        else {
            snprintf(buffer, sizeof(buffer), "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"/>\n", center->x, center->y, radius);
        }

    // Write the XML string using context->write_fn(context->user, buffer) and the error will be check by google test or something i dont think i should return SVG_OK
    return context->write_fn(context->user, buffer);
    
}


svg_return_t svg_rect(svg_context_ptr context,
                      const svg_point_t *top_left,
                      const svg_size_t *size,
                      const char* style){
    // Enter code here

    // Checks if context is NULL
    if (!context) {
        return SVG_ERR_NULL;
    }
    if (!top_left || !size) {
        return SVG_ERR_NULL;
    }

    // Creates the buffer and then creates the rectangle with all the function params and using a conditional to account for NULL styles
    char buffer[256];

    if(style) {
        snprintf(buffer, sizeof(buffer), "<rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" style=\"%s\"/>\n", top_left->x, top_left->y, size->width, size->height, style);
    }
    else {
        snprintf(buffer, sizeof(buffer), "<rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\"/>\n", top_left->x, top_left->y, size->width, size->height);
    }
    
    // Return whats inside of the buffer (the XML command)
    return context->write_fn(context->user, buffer);
    

}

svg_return_t svg_line(svg_context_ptr context,
                      const svg_point_t *start,
                      const svg_point_t *end,
                      const char* style){
    // Enter code here
    
    // Checks if context is NULL
    if (!context) {
        return SVG_ERR_NULL;
    }
    if (!start || !end) {
        return SVG_ERR_NULL;
    }
    
    // Creating the XML output into the buffer and using a conditional to account for NULL styles
    char buffer[256];
    if (style) {
    snprintf(buffer, sizeof(buffer), "<line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\" style=\"%s\"/>\n", start->x, start->y, end->x, end->y, style);
    }
    else { 
    snprintf(buffer, sizeof(buffer), "<line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\" />\n", start->x, start->y, end->x, end->y);
    }

    return context->write_fn(context->user, buffer);
}

svg_return_t svg_group_begin(svg_context_ptr context, 
                             const char* attrs){
    // Enter code here
    
     // Checks if context is NULL
    if (!context) {
        return SVG_ERR_NULL;
    }

    // Creating the Buffer
    char buffer[512];

        if(attrs) {
             snprintf(buffer, sizeof(buffer), "<g %s>\n", attrs);
        }
        else {
            snprintf(buffer, sizeof(buffer), "<g>\n");
        }

        return context->write_fn(context->user, buffer);



}

svg_return_t svg_group_end(svg_context_ptr context){
    // Enter code here

    // Checks if context is NULL
    if (!context) {
        return SVG_ERR_NULL;
    }
    
    // No need for buffer because this will end this way everytime
    return context->write_fn(context->user, "</g>\n");
    
}
