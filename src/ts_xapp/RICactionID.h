/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-IEs"
 * 	found in "/home/sjana/ASN-Defns/e2ap-oran-wg3-v01.00.asn"
 * 	`asn1c -fno-include-deps -fcompound-names -findirect-choice -gen-PER -no-gen-OER`
 */

#ifndef	_RICactionID_H_
#define	_RICactionID_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RICactionID */
typedef long	 RICactionID_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_RICactionID_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_RICactionID;
asn_struct_free_f RICactionID_free;
asn_struct_print_f RICactionID_print;
asn_constr_check_f RICactionID_constraint;
ber_type_decoder_f RICactionID_decode_ber;
der_type_encoder_f RICactionID_encode_der;
xer_type_decoder_f RICactionID_decode_xer;
xer_type_encoder_f RICactionID_encode_xer;
per_type_decoder_f RICactionID_decode_uper;
per_type_encoder_f RICactionID_encode_uper;
per_type_decoder_f RICactionID_decode_aper;
per_type_encoder_f RICactionID_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _RICactionID_H_ */
#include "asn_internal.h"
