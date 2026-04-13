export function currency(value) {
  return new Intl.NumberFormat("en-IN", {
    style: "currency",
    currency: "INR",
    maximumFractionDigits: 0,
  }).format(Number(value || 0));
}

export function titleCase(text = "") {
  return text
    .toLowerCase()
    .split("_")
    .map((t) => t.charAt(0).toUpperCase() + t.slice(1))
    .join(" ");
}
