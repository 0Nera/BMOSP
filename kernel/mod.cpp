#include <fb.h>
#include <limine.h>
#include <mod.h>

namespace mod {
static volatile struct limine_module_request module_request = {
	.id = LIMINE_MODULE_REQUEST,
	.revision = 0,
	.response = (struct limine_module_response *)0
};

struct limine_module_response *module_response;
static uint64_t module_count = 0;

void init( ) {
	module_response = module_request.response;
	module_count = module_response->module_count;
	struct limine_file *module_ptr = (struct limine_file *)0;

	for (uint64_t i = 0; i < module_count; i++) {
		module_ptr = module_response->modules[i];
		fb::printf("[%d] %s [%s] 0x%x\n", i, module_ptr->path,
		           module_ptr->cmdline, module_ptr->address);
		fb::printf("->Size: %u, media_type: %u, partition_index: %u\n",
		           module_ptr->size, module_ptr->media_type,
		           module_ptr->partition_index);
		fb::printf("->mbr_disk_id: %u, tftp_ip: %u, tftp_port: %u\n\n",
		           module_ptr->mbr_disk_id, module_ptr->tftp_ip,
		           module_ptr->tftp_port);
	}
}
} // namespace mod