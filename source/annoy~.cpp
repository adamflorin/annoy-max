/**
* annoy~.cpp: wrapper around the annoy Approximate Nearest Neighbor search
* https://github.com/spotify/annoy
*
* Copyright 2018 Adam Florin
*/

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#include "kissrandom.h"
#include "annoylib.h"

extern "C" {
	// External struct
	typedef struct _annoy {
		t_pxobject object;
		AnnoyIndex<int, float, Angular, Kiss32Random> *index;
	} t_annoy;

	// Method prototypes
	void *annoy_new(t_symbol *s, long argc, t_atom *argv);
	void annoy_free(t_annoy *x);
	void annoy_assist(t_annoy *x, void *b, long m, long a, char *s);
	void annoy_dsp64(
		t_annoy *x,
		t_object *dsp64,
		short *count,
		double samplerate,
		long maxvectorsize,
		long flags
	);
	void annoy_perform64(
		t_annoy *x,
		t_object *dsp64,
		double **ins,
		long numins,
		double **outs,
		long numouts,
		long sampleframes,
		long flags,
		void *userparam
	);

	// External class
	static t_class *annoy_class = NULL;

	/** Initialize external class */
	void ext_main(void *r) {
		t_class *c = class_new(
			"annoy~",
			(method)annoy_new,
			(method)annoy_free,
			(long)sizeof(t_annoy),
			0L,
			A_GIMME,
			0
		);

		class_addmethod(c, (method)annoy_dsp64, "dsp64", A_CANT, 0);
		class_addmethod(c, (method)annoy_assist, "assist", A_CANT, 0);

		class_dspinit(c);
		class_register(CLASS_BOX, c);
		annoy_class = c;
	}

	/** Initialize external instance */
	void *annoy_new(t_symbol *s, long argc, t_atom *argv) {
		t_annoy *x = (t_annoy *)object_alloc(annoy_class);

		if (!x) {
			return x;
		}

		// I/O
		dsp_setup((t_pxobject *)x, 1);
		outlet_new(x, "signal");

		// annoy
		int f = 20;
		// template<typename S, typename T, typename Distance, typename Random>
		x->index = new AnnoyIndex<int, float, Angular, Kiss32Random>(f);
		bool loaded = x->index->load("/Users/adam/local/projects/phys-ex/code/radio-concrete-no-2/db/nsynth.ann");

		if (!loaded) {
			object_error(
				(t_object*)x,
				"Failed to load annoy file."
			);
		}

		/*
		// TEMP! TEST sample input to validate
		int num_neighbors = 2;
		const float input[20] = {
			-138.810237,
			111.241042,
			-30.094307,
			34.878496,
			12.335299,
			-4.960763,
			-5.432953,
			-22.720864,
			-9.809753,
			-29.613856,
			-0.797928,
			-23.251343,
			-2.311884,
			1.106706,
			-17.230525,
			-9.372263,
			-20.003280,
			6.339963,
			-3.546490,
			11.745176
		};
		std::vector<int> matches;
		std::vector<float> distances;

		x->index->get_nns_by_vector(
			input,					// const T* w
			num_neighbors,	// size_t n
			(size_t) -1,		// size_t search_k - defaults to "n_trees * n" if not provided
			&matches, 			// vector<S>* result,
			&distances			// vector<T>* distances
		);
		*/

		return x;
	}

	/** Destroy external instance */
	void annoy_free(t_annoy *x) {
		dsp_free((t_pxobject *)x);
	}

	/** Configure user tooltip prompts */
	void annoy_assist(t_annoy *x, void *b, long m, long a, char *s) {
		if (m == ASSIST_INLET) {
			sprintf(s, "I am inlet %ld", a);
		} else {
			sprintf(s, "I am outlet %ld", a);
		}
	}

	/** Register perform method */
	void annoy_dsp64(
		t_annoy *x,
		t_object *dsp64,
		short *count,
		double samplerate,
		long maxvectorsize,
		long flags
	) {
		post("my sample rate is: %f", samplerate);

		object_method(dsp64, gensym("dsp_add64"), x, annoy_perform64, 0, NULL);
	}

	/** Perform DSP */
	void annoy_perform64(
		t_annoy *x,
		t_object *dsp64,
		double **ins,
		long numins,
		double **outs,
		long numouts,
		long sampleframes,
		long flags,
		void *userparam
	) {
		t_double *inL = ins[0];
		t_double *outL = outs[0];
		int n = sampleframes;

		while (n--) {
			*outL++ = *inL++;
		}
	}
}
