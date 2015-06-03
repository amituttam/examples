
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libkmod.h>

static int load_module(struct kmod_ctx *ctx, const char *m) {
	struct kmod_list *itr, *modlist = NULL;
	const int probe_flags = KMOD_PROBE_APPLY_BLACKLIST;
	const char *options = "luns=8 iSerialNumber=1234000 ro=1";
	int r = 0;

	r = kmod_module_new_from_lookup(ctx, m, &modlist);
	if (r < 0) {
		printf("ERROR: Failed to lookup alias '%s': %s\n", m, strerror(-r));
	}

	if (!modlist) {
		printf("ERROR: Failed to find module '%s'\n", m);
		return -ENOENT;
	}

	 kmod_list_foreach(itr, modlist) {
		struct kmod_module *mod;
		int state, err;

		mod = kmod_module_get_module(itr);
		state = kmod_module_get_initstate(mod);

		switch (state) {
			case KMOD_MODULE_BUILTIN:
				printf("Module '%s' is builtin\n", kmod_module_get_name(mod));
				break;
			case KMOD_MODULE_LIVE:
				printf("Module '%s' is already loaded\n", kmod_module_get_name(mod));
				//printf("Unloading module '%s'...\n", kmod_module_get_name(mod));

				//err = kmod_module_remove_module(mod, KMOD_REMOVE_FORCE);

				//if (err == 0)
				//{
				//	printf("Successfully removed module '%s'\n", kmod_module_get_name(mod));
				//	r = EAGAIN;
				//}
				//else
				//{
				//	printf("ERROR: Module '%s' can't be removed\n", kmod_module_get_name(mod));
				//	r = err;
				//}

				break;
			default:
				err = kmod_module_probe_insert_module(mod, probe_flags, options, NULL, NULL, NULL);

				if (err == 0)
					printf("Inserted module '%s'\n", kmod_module_get_name(mod));
				else if (err == KMOD_PROBE_APPLY_BLACKLIST)
					printf("Module '%s' is blacklisted\n", kmod_module_get_name(mod));
				else {
					printf("ERROR: Failed to insert '%s': %s\n", kmod_module_get_name(mod), strerror(-err));
					r = err;
				}
		}

		kmod_module_unref(mod);
	 }

	 kmod_module_unref_list(modlist);

	 return r;
}

int main( int argc, char **argv )
{
	int r;
	struct kmod_ctx *ctx;

	ctx = kmod_new(NULL, NULL);
	if (!ctx) {
		printf("Failed to allocate memory for kmod\n");
	}

	kmod_load_resources(ctx);

	/* Load g_mass_storage module */
	r = load_module(ctx, "g_mass_storage");
	if (r == EAGAIN)
		load_module(ctx, "g_mass_storage");


	kmod_unref(ctx);
	return 0;
}

/* colorgcc -Wall -lkmod -o kmod kmod.c */
