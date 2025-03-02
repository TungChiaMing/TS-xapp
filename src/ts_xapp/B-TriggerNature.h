/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-Bouncer-IEs"
 *      found in "/home/ubuntu/ASN-Defns/e2sm-Bouncer-v002.asn"
 * 	`asn1c -fno-include-deps -fcompound-names -findirect-choice -gen-PER -no-gen-OER`
 */

#ifndef	_B_TriggerNature_H_
#define	_B_TriggerNature_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeEnumerated.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum B_TriggerNature {
	B_TriggerNature_now	= 0,
	B_TriggerNature_onchange	= 1
	/*
	 * Enumeration is extensible
	 */
} e_B_TriggerNature;

/* B-TriggerNature */
typedef long	 B_TriggerNature_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_B_TriggerNature_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_B_TriggerNature;
extern const asn_INTEGER_specifics_t asn_SPC_B_TriggerNature_specs_1;
asn_struct_free_f B_TriggerNature_free;
asn_struct_print_f B_TriggerNature_print;
asn_constr_check_f B_TriggerNature_constraint;
ber_type_decoder_f B_TriggerNature_decode_ber;
der_type_encoder_f B_TriggerNature_encode_der;
xer_type_decoder_f B_TriggerNature_decode_xer;
xer_type_encoder_f B_TriggerNature_encode_xer;
per_type_decoder_f B_TriggerNature_decode_uper;
per_type_encoder_f B_TriggerNature_encode_uper;
per_type_decoder_f B_TriggerNature_decode_aper;
per_type_encoder_f B_TriggerNature_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _B_TriggerNature_H_ */
#include "asn_internal.h"
