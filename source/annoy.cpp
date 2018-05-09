/**
* annoy.cpp: wrapper around the annoy Approximate Nearest Neighbor search
* https://github.com/spotify/annoy
*
* Copyright 2018 Adam Florin
*/

#include "ext.h"
#include "ext_obex.h"

#include "annoylib.h"
#include "kissrandom.h"

extern "C" {

	const long DEFAULT_NUM_DIMENSIONS = 20;
	const long DEFAULT_NUM_NEIGHBORS = 2;

	// External struct
	typedef struct _annoy {
		t_object object;
		long num_dimensions = DEFAULT_NUM_DIMENSIONS;
		AnnoyIndex<int, float, Angular, Kiss32Random> *index;
		void *neighbors_outlet;
		void *distances_outlet;
	} t_annoy;

	// Method prototypes
	void *annoy_new(t_symbol *s, long argc, t_atom *argv);
	void annoy_free(t_annoy *x);
	void annoy_assist(t_annoy *x, void *b, long m, long a, char *s);
	void annoy_load(t_annoy *x, t_symbol *s);
	void annoy_unload(t_annoy *x);
	void annoy_get_nns_by_item(t_annoy *x, long n);
	void annoy_get_nns_by_vector(t_annoy *x, t_symbol *s, long argc, t_atom *argv);
	void annoy_output_neighbors(t_annoy *x, std::vector<int> &neighbors, std::vector<float> &distances);

	// External class
	static t_class *annoy_class = NULL;

	/** Initialize external class */
	void ext_main(void *r) {
		t_class *c = class_new(
			"annoy",
			(method)annoy_new,
			(method)annoy_free,
			(long)sizeof(t_annoy),
			0L,
			A_GIMME,
			0
		);

		class_addmethod(c, (method)annoy_assist, "assist", A_CANT, 0);
		class_addmethod(c, (method)annoy_load, "load", A_SYM, 0);
		class_addmethod(c, (method)annoy_unload, "unload", 0);
		class_addmethod(c, (method)annoy_get_nns_by_item, "get_nns_by_item", A_LONG, 0);
		class_addmethod(c, (method)annoy_get_nns_by_vector, "get_nns_by_vector", A_GIMME, 0);

		class_register(CLASS_BOX, c);
		annoy_class = c;
	}

	/** Initialize external instance */
	void *annoy_new(t_symbol *s, long argc, t_atom *argv) {
		t_annoy *x = (t_annoy *)object_alloc(annoy_class);

		if (!x) {
			return x;
		}

		x->distances_outlet = listout(x);
		x->neighbors_outlet = listout(x);

		if (argc > 0) {
			if (atom_gettype(argv) == A_LONG) {
				x->num_dimensions = atom_getlong(argv);
			}
		}
		x->index = new AnnoyIndex<int, float, Angular, Kiss32Random>(x->num_dimensions);

		return x;
	}

	/** Destroy external instance */
	void annoy_free(t_annoy *x) {
		annoy_unload(x);
	}

	/** Configure user tooltip prompts */
	void annoy_assist(t_annoy *x, void *b, long m, long a, char *s) {
		if (m == ASSIST_INLET) {
			sprintf(s, "I am inlet %ld", a);
		} else {
			sprintf(s, "I am outlet %ld", a);
		}
	}

	void annoy_load(t_annoy *x, t_symbol *s) {
		bool loaded = x->index->load(s->s_name);
		if (loaded) {
			object_post((t_object*)x, "Loaded annoy file '%s'", s->s_name);
		} else {
			object_error((t_object*)x, "Failed to load annoy file '%s'", s->s_name);
		}
	}

	void annoy_unload(t_annoy *x) {
		x->index->unload();
	}

	void annoy_get_nns_by_item(t_annoy *x, long item) {
		std::vector<int> neighbors;
		std::vector<float> distances;
		x->index->get_nns_by_item(
			item,
			DEFAULT_NUM_NEIGHBORS,
			(size_t) -1,
			&neighbors,
			&distances
		);
		annoy_output_neighbors(x, neighbors, distances);
	}

	void annoy_get_nns_by_vector(t_annoy *x, t_symbol *s, long argc, t_atom *argv) {
		float vector[x->num_dimensions];
		long i;
		for (i = 0; i < std::min(argc, x->num_dimensions); i++) {
			vector[i] = atom_getfloat(&argv[i]);
		}
		std::vector<int> neighbors;
		std::vector<float> distances;
		x->index->get_nns_by_vector(
			(const float *)vector,
			DEFAULT_NUM_NEIGHBORS,
			(size_t) -1,
			&neighbors,
			&distances
		);
		annoy_output_neighbors(x, neighbors, distances);
	}

	void annoy_output_neighbors(t_annoy *x, std::vector<int> &neighbors, std::vector<float> &distances) {
		long i;
		t_atom neighbor_atoms[DEFAULT_NUM_NEIGHBORS];
		t_atom distance_atoms[DEFAULT_NUM_NEIGHBORS];

		for (i = 0; i < DEFAULT_NUM_NEIGHBORS; i++) {
			atom_setlong(neighbor_atoms + i, neighbors[i]);
			atom_setfloat(distance_atoms + i, distances[i]);
		}

		outlet_list(x->neighbors_outlet, 0L, DEFAULT_NUM_NEIGHBORS, neighbor_atoms);
		outlet_list(x->distances_outlet, 0L, DEFAULT_NUM_NEIGHBORS, distance_atoms);
	}
}
