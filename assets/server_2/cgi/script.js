const form = document.getElementById("cgi_form");
const methodSelect = document.querySelector('select[name="method"]');
const languageSelect = document.querySelector('select[name="language"]');
const enctypeSelect = document.querySelector('select[name="enctype"]');
const filename = "default";

form.addEventListener("submit", (event) => {
	event.preventDefault(); // Prevent default form submission

	const selectedMethod = methodSelect.value;
	const selectedLanguage = languageSelect.value;
	const selectedEnctype = enctypeSelect.value;

	// Update form attributes
	form.method = selectedMethod;
	var extention;
	if (selectedLanguage == "python")
		extention = ".py";
	else if (selectedLanguage == "bash")
		extention = ".sh";
	else if (selectedLanguage == "php")
		extention = ".php";
	form.action = `/cgi-bin/${filename}${extention}`;
	form.enctype = selectedEnctype;


    form.submit();
});
