/**
 * Initiates the capture plus object.
 */
(function ($) {
    Drupal.behaviors.webform_captureplus_admin = Drupal.behaviors.webform_captureplus_admin || {};
    Drupal.behaviors.webform_captureplus_admin = function(context) {
        $(document).ready(function(){
            var $mode = '';
            $('#webform-components .mode').each(function(index){
                placeholderElement($(this));
            });
            $('#webform-components .mode').change(function(){
                placeholderElement($(this));
            })
        });


        function placeholderElement($element) {
            var $mode = $element.find(":selected").val();
            if ($mode == 1 || $mode == 3) {
                $('.placeholder', $element.parent()).show();
            }
            else {
                $('.placeholder', $element.parent()).hide();
            }
        }
    };

}(jQuery));
