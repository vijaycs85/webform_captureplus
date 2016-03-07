/**
 * Initiates the capture plus object.
 */
(function ($) {
    Drupal.behaviors.webform_captureplus_admin = Drupal.behaviors.webform_captureplus_admin || {};
    Drupal.behaviors.webform_captureplus_admin.attach = function() {
        $(document).ready(function(){
            $('#webform-components .mode').each(function(index){
                placeholderElement($(this));
               $(this).change(function(){
                   placeholderElement($(this));
               });
            });
        });


        function placeholderElement($element) {

            var $mode = $element.find(":selected").val();
            if ($mode == 1 || $mode == 3) {
                $('.placeholder', $element.parents('tr')).show();
            }
            else {
                $('.placeholder', $element.parents('tr')).hide();
            }
        }
    };

}(jQuery));
