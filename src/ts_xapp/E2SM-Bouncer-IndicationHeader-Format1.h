/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-Bouncer-IEs"
 *      found in "/home/ubuntu/ASN-Defns/e2sm-Bouncer-v002.asn"
 * 	`asn1c -fno-include-deps -fcompound-names -findirect-choice -gen-PER -no-gen-OER`
 */

#ifndef	_E2SM_Bouncer_IndicationHeader_Format1_H_
#define	_E2SM_Bouncer_IndicationHeader_Format1_H_


#include "asn_application.h"

/* Including external dependencies */
#include "B-Header.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* E2SM-Bouncer-IndicationHeader-Format1 */
typedef struct E2SM_Bouncer_IndicationHeader_Format1 {
	B_Header_t	 indicationHeaderParam;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} E2SM_Bouncer_IndicationHeader_Format1_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_E2SM_Bouncer_IndicationHeader_Format1;
extern asn_SEQUENCE_specifics_t asn_SPC_E2SM_Bouncer_IndicationHeader_Format1_specs_1;
extern asn_TYPE_member_t asn_MBR_E2SM_Bouncer_IndicationHeader_Format1_1[1];

#ifdef __cplusplus
}
#endif

#endif	/* _E2SM_Bouncer_IndicationHeader_Format1_H_ */
#include "asn_internal.h"
