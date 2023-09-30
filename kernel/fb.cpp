#include <limine.h>



namespace fb {
    static volatile struct limine_framebuffer_request framebuffer_request = {
        .id = LIMINE_FRAMEBUFFER_REQUEST,
        .revision = 0,
        .response = (struct limine_framebuffer_response *)0
    };

    struct limine_framebuffer_response *framebuffer_response;
    struct limine_framebuffer *boot_framebuffer;
    uint32_t *fb_addr;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    
    void init() {
        framebuffer_response = framebuffer_request.response;
        boot_framebuffer = framebuffer_response->framebuffers[0];
        fb_addr = (uint32_t*)boot_framebuffer->address;
        width = boot_framebuffer->width;
        height = boot_framebuffer->height;
        bpp = boot_framebuffer->bpp;

        for (uint64_t i = 0; i < width * height; i++) {
            fb_addr[i] = 0x0000FF;
        }
    }
}