/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-CommonDataTypes"
 * 	found in "/home/sjana/ASN-Defns/e2ap-oran-wg3-v01.00.asn"
 * 	`asn1c -fno-include-deps -fcompound-names -findirect-choice -gen-PER -no-gen-OER`
 */

#ifndef	_TriggeringMessage_H_
#define	_TriggeringMessage_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeEnumerated.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum TriggeringMessage {
	TriggeringMessage_initiating_message	= 0,
	TriggeringMessage_successful_outcome	= 1,
	TriggeringMessage_unsuccessfull_outcome	= 2
} e_TriggeringMessage;

/* TriggeringMessage */
typedef long	 TriggeringMessage_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_TriggeringMessage_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_TriggeringMessage;
extern const asn_INTEGER_specifics_t asn_SPC_TriggeringMessage_specs_1;
asn_struct_free_f TriggeringMessage_free;
asn_struct_print_f TriggeringMessage_print;
asn_constr_check_f TriggeringMessage_constraint;
ber_type_decoder_f TriggeringMessage_decode_ber;
der_type_encoder_f TriggeringMessage_encode_der;
xer_type_decoder_f TriggeringMessage_decode_xer;
xer_type_encoder_f TriggeringMessage_encode_xer;
per_type_decoder_f TriggeringMessage_decode_uper;
per_type_encoder_f TriggeringMessage_encode_uper;
per_type_decoder_f TriggeringMessage_decode_aper;
per_type_encoder_f TriggeringMessage_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _TriggeringMessage_H_ */
#include "asn_internal.h"
